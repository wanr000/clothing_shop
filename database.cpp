#include "database.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <libpq-fe.h>

Database::Database(const std::string& connStr) : connection(nullptr), connectionString(connStr) {
}

Database::~Database() {
    disconnect();
}


bool Database::connect() {
    connection = PQconnectdb(connectionString.c_str());
    
    if (PQstatus(connection) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(connection) << std::endl;
        PQfinish(connection);
        connection = nullptr;
        return false;
    }
    
    std::cout << "Connected to PostgreSQL database successfully!" << std::endl;
    return true;
}

void Database::disconnect() {
    if (connection) {
        PQfinish(connection);
        connection = nullptr;
    }
}



bool Database::initialize() {
    if (!connect()) {
        std::cerr << "Failed to connect to database" << std::endl;
        return false;
    }
    
    createTables();
    
    PGresult* res = executeQuery("SELECT COUNT(*) FROM users");
    if (res) {
        if (PQntuples(res) > 0) {
            const char* countStr = PQgetvalue(res, 0, 0);
            int count = countStr ? std::atoi(countStr) : 0;
            PQclear(res);
            
            if (count == 0) {
                std::cout << "Creating sample data..." << std::endl;
                createSampleData();
            }
        } else {
            PQclear(res);
        }
    }
    
    std::cout << "Database initialized successfully!" << std::endl;
    return true;
}

// В методе createTables() добавим создание таблицы feedbacks
void Database::createTables() {
    std::string sql_users = R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            username VARCHAR(50) UNIQUE NOT NULL,
            password VARCHAR(100) NOT NULL,
            role INTEGER NOT NULL,
            balance FLOAT DEFAULT 0.0,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    std::string sql_products = R"(
        CREATE TABLE IF NOT EXISTS products (
            id SERIAL PRIMARY KEY,
            name VARCHAR(100) NOT NULL,
            category INTEGER NOT NULL,
            price FLOAT NOT NULL,
            quantity INTEGER NOT NULL,
            size VARCHAR(20),
            color VARCHAR(30),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    std::string sql_orders = R"(
        CREATE TABLE IF NOT EXISTS orders (
            id SERIAL PRIMARY KEY,
            user_id INTEGER REFERENCES users(id),
            total_amount FLOAT NOT NULL,
            status VARCHAR(20) DEFAULT 'pending',
            order_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    std::string sql_order_items = R"(
        CREATE TABLE IF NOT EXISTS order_items (
            id SERIAL PRIMARY KEY,
            order_id INTEGER REFERENCES orders(id),
            product_id INTEGER REFERENCES products(id),
            quantity INTEGER NOT NULL,
            price_at_time FLOAT NOT NULL
        )
    )";
    
    // ДОБАВЛЕНО: Таблица для отзывов
    std::string sql_feedbacks = R"(
        CREATE TABLE IF NOT EXISTS feedbacks (
            id SERIAL PRIMARY KEY,
            user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
            username VARCHAR(50) NOT NULL,
            feedback TEXT NOT NULL,
            rating INTEGER NOT NULL CHECK (rating >= 1 AND rating <= 5),
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    executeSQL(sql_users);
    executeSQL(sql_products);
    executeSQL(sql_orders);
    executeSQL(sql_order_items);
    executeSQL(sql_feedbacks); // ДОБАВЛЕНО
}

// В методе createSampleData() добавим тестовые отзывы
void Database::createSampleData() {
    std::cout << "Creating sample data..." << std::endl;
    
    // Добавляем администратора
    if (!registerUser("admin", "admin", User::Role::SELLER, 0.0f)) {
        std::cerr << "Failed to create admin user" << std::endl;
    }
    
    // Добавляем тестового покупателя с начальным балансом
    if (!registerUser("customer", "password", User::Role::CUSTOMER, 1000.0f)) {
        std::cerr << "Failed to create customer user" << std::endl;
    }
    
    // Добавляем еще покупателей
    registerUser("john", "123456", User::Role::CUSTOMER, 500.0f);
    registerUser("mary", "password123", User::Role::CUSTOMER, 750.0f);
    
    // Добавляем товары (автомобили)
    Product products[] = {
        Product(0, "Lamborghini Aventador", Product::Category::SUPERCAR, 450000.0f, 5, "730 HP", "Orange"),
        Product(0, "Ferrari F8 Tributo", Product::Category::SUPERCAR, 350000.0f, 7, "720 HP", "Red"),
        Product(0, "Bugatti Chiron", Product::Category::PANTS, 3500000.0f, 2, "1500 HP", "Blue"),
        Product(0, "Koenigsegg Jesko", Product::Category::DRESS, 3000000.0f, 3, "1600 HP", "Silver"),
        Product(0, "Toyota Camry", Product::Category::ACCESSORIES, 30000.0f, 15, "203 HP", "White"),
        Product(0, "Honda Accord", Product::Category::PANTS, 32000.0f, 12, "192 HP", "Black"),
        Product(0, "F1 McLaren", Product::Category::SHOES, 15000000.0f, 1, "1000+ HP", "Papaya Orange"),
        Product(0, "Ford GT Le Mans", Product::Category::SHOES, 500000.0f, 4, "647 HP", "Gulf Blue"),
        Product(0, "Shelby Cobra 427", Product::Category::SHOES, 2000000.0f, 2, "425 HP", "Blue/White"),
        Product(0, "Toyota Corolla", Product::Category::ACCESSORIES, 25000.0f, 20, "169 HP", "Gray")
    };
    
    for (const auto& product : products) {
        if (!addProduct(product)) {
            std::cerr << "Failed to add product: " << product.getName() << std::endl;
        }
    }
    
    // ДОБАВЛЕНО: Тестовые отзывы
    std::cout << "Creating sample feedbacks..." << std::endl;
    
    // Получаем ID пользователей
    auto adminUser = getUserByUsername("admin");
    auto customerUser = getUserByUsername("customer");
    auto johnUser = getUserByUsername("john");
    auto maryUser = getUserByUsername("mary");
    
    // Добавляем тестовые отзывы если пользователи существуют
    if (adminUser) {
        addFeedback(adminUser->getId(), "Great service! Fast delivery and excellent quality cars.", 5);
    }
    
    if (customerUser) {
        addFeedback(customerUser->getId(), "Bought a Lamborghini, amazing experience! The staff was very helpful.", 5);
        addFeedback(customerUser->getId(), "Shipping took a bit longer than expected, but the car was worth the wait.", 4);
    }
    
    if (johnUser) {
        addFeedback(johnUser->getId(), "Best car dealership ever! Got my dream Ferrari.", 5);
        addFeedback(johnUser->getId(), "Customer support could be better in responding to emails.", 3);
    }
    
    if (maryUser) {
        addFeedback(maryUser->getId(), "Love my new Toyota Camry! Perfect family car.", 4);
        addFeedback(maryUser->getId(), "The website is easy to use and the selection is great.", 5);
    }
    
    std::cout << "Sample data created successfully!" << std::endl;
}

// ===== МЕТОДЫ ДЛЯ РАБОТЫ С ОТЗЫВАМИ =====

bool Database::addFeedback(int userId, const std::string& feedback, int rating) {
    try {
        // Проверяем рейтинг
        if (rating < 1 || rating > 5) {
            std::cerr << "Rating must be between 1 and 5" << std::endl;
            return false;
        }
        
        // Получаем информацию о пользователе
        auto user = getUserById(userId);
        if (!user) {
            std::cerr << "User not found with ID: " << userId << std::endl;
            return false;
        }
        
        std::string username = user->getUsername();
        
        // Подготавливаем SQL запрос
        std::stringstream sql;
        sql << "INSERT INTO feedbacks (user_id, username, feedback, rating) VALUES ("
            << userId << ", '"
            << escapeString(username) << "', '"
            << escapeString(feedback) << "', "
            << rating << ")";
        
        bool success = executeSQL(sql.str());
        if (success) {
            std::cout << "Feedback added from user: " << username << " (Rating: " << rating << "/5)" << std::endl;
        } else {
            std::cerr << "Failed to add feedback" << std::endl;
        }
        
        return success;
    } catch (const std::exception& e) {
        std::cerr << "Error adding feedback: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Feedback> Database::getRecentFeedbacks(int limit) {
    std::vector<Feedback> feedbacks;
    
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return feedbacks;
        }
        
        std::stringstream sql;
        sql << "SELECT id, user_id, username, feedback, rating, created_at "
            << "FROM feedbacks "
            << "ORDER BY created_at DESC "
            << "LIMIT " << limit;
        
        PGresult* res = executeQuery(sql.str());
        if (!res) {
            return feedbacks;
        }
        
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            feedbacks.push_back(createFeedbackFromRow(res, i));
        }
        
        PQclear(res);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting recent feedbacks: " << e.what() << std::endl;
    }
    
    return feedbacks;
}

std::vector<Feedback> Database::getUserFeedbacks(int userId) {
    std::vector<Feedback> feedbacks;
    
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return feedbacks;
        }
        
        std::stringstream sql;
        sql << "SELECT id, user_id, username, feedback, rating, created_at "
            << "FROM feedbacks "
            << "WHERE user_id = " << userId << " "
            << "ORDER BY created_at DESC";
        
        PGresult* res = executeQuery(sql.str());
        if (!res) {
            return feedbacks;
        }
        
        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            feedbacks.push_back(createFeedbackFromRow(res, i));
        }
        
        PQclear(res);
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting user feedbacks: " << e.what() << std::endl;
    }
    
    return feedbacks;
}

std::vector<Feedback> Database::getProductFeedbacks(int productId) {
    // В этой версии отзывы не привязаны к конкретным товарам
    // Можно расширить в будущем, добавив product_id в таблицу feedbacks
    std::vector<Feedback> feedbacks;
    std::cout << "Note: Product-specific feedbacks not implemented yet" << std::endl;
    return feedbacks;
}

float Database::getAverageRating() {
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return 0.0f;
        }
        
        std::string sql = "SELECT AVG(rating) as avg_rating FROM feedbacks";
        PGresult* res = executeQuery(sql);
        
        if (!res || PQntuples(res) == 0) {
            if (res) PQclear(res);
            return 0.0f;
        }
        
        const char* avgStr = PQgetvalue(res, 0, 0);
        float average = 0.0f;
        
        if (avgStr) {
            average = std::atof(avgStr);
        }
        
        PQclear(res);
        return average;
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting average rating: " << e.what() << std::endl;
        return 0.0f;
    }
}

int Database::getFeedbackCount() {
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return 0;
        }
        
        std::string sql = "SELECT COUNT(*) as count FROM feedbacks";
        PGresult* res = executeQuery(sql);
        
        if (!res || PQntuples(res) == 0) {
            if (res) PQclear(res);
            return 0;
        }
        
        const char* countStr = PQgetvalue(res, 0, 0);
        int count = 0;
        
        if (countStr) {
            count = std::atoi(countStr);
        }
        
        PQclear(res);
        return count;
        
    } catch (const std::exception& e) {
        std::cerr << "Error getting feedback count: " << e.what() << std::endl;
        return 0;
    }
}

bool Database::deleteFeedback(int feedbackId) {
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return false;
        }
        
        std::stringstream sql;
        sql << "DELETE FROM feedbacks WHERE id = " << feedbackId;
        
        bool success = executeSQL(sql.str());
        
        if (success) {
            std::cout << "Feedback deleted successfully (ID: " << feedbackId << ")" << std::endl;
        } else {
            std::cerr << "Failed to delete feedback (ID: " << feedbackId << ")" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "Error deleting feedback: " << e.what() << std::endl;
        return false;
    }
}

bool Database::updateFeedback(int feedbackId, const std::string& newFeedback, int newRating) {
    try {
        if (!connection) {
            std::cerr << "No database connection" << std::endl;
            return false;
        }
        
        // Проверяем рейтинг
        if (newRating < 1 || newRating > 5) {
            std::cerr << "Rating must be between 1 and 5" << std::endl;
            return false;
        }
        
        std::stringstream sql;
        sql << "UPDATE feedbacks SET "
            << "feedback = '" << escapeString(newFeedback) << "', "
            << "rating = " << newRating << ", "
            << "created_at = CURRENT_TIMESTAMP "
            << "WHERE id = " << feedbackId;
        
        bool success = executeSQL(sql.str());
        
        if (success) {
            std::cout << "Feedback updated successfully (ID: " << feedbackId << ")" << std::endl;
        } else {
            std::cerr << "Failed to update feedback (ID: " << feedbackId << ")" << std::endl;
        }
        
        return success;
        
    } catch (const std::exception& e) {
        std::cerr << "Error updating feedback: " << e.what() << std::endl;
        return false;
    }
}

Feedback Database::createFeedbackFromRow(PGresult* res, int row) {
    try {
        const char* idStr = PQgetvalue(res, row, 0);
        const char* userIdStr = PQgetvalue(res, row, 1);
        const char* username = PQgetvalue(res, row, 2);
        const char* feedbackText = PQgetvalue(res, row, 3);
        const char* ratingStr = PQgetvalue(res, row, 4);
        const char* createdAt = PQgetvalue(res, row, 5);
        
        if (!idStr || !userIdStr || !username || !feedbackText || !ratingStr || !createdAt) {
            return Feedback();
        }
        
        int id = std::atoi(idStr);
        int userId = std::atoi(userIdStr);
        int rating = std::atoi(ratingStr);
        
        return Feedback(id, userId, username, feedbackText, rating, createdAt);
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating feedback from row: " << e.what() << std::endl;
        return Feedback();
    }
}

std::shared_ptr<User> Database::authenticateUser(const std::string& username, const std::string& password) {
    std::string sql = "SELECT id, username, password, role, balance FROM users WHERE username = '" +
                     escapeString(username) + "' AND password = '" + escapeString(password) + "'";
    
    PGresult* res = executeQuery(sql);
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return nullptr;
    }
    
    auto user = createUserFromRow(res, 0);
    PQclear(res);
    return user;
}

bool Database::registerUser(const std::string& username, const std::string& password, User::Role role, float balance) {
    std::string checkSql = "SELECT id FROM users WHERE username = '" + escapeString(username) + "'";
    PGresult* checkRes = executeQuery(checkSql);
    if (checkRes && PQntuples(checkRes) > 0) {
        PQclear(checkRes);
        std::cout << "User already exists: " << username << std::endl;
        return false;
    }
    if (checkRes) PQclear(checkRes);
    
    std::stringstream sql;
    sql << "INSERT INTO users (username, password, role, balance) VALUES ('"
        << escapeString(username) << "', '"
        << escapeString(password) << "', "
        << static_cast<int>(role) << ", "
        << balance << ")";
    
    bool success = executeSQL(sql.str());
    if (success) {
        std::cout << "User registered successfully: " << username << std::endl;
    }
    return success;
}

std::shared_ptr<User> Database::getUserByUsername(const std::string& username) {
    std::string sql = "SELECT id, username, password, role, balance FROM users WHERE username = '" +
                     escapeString(username) + "'";
    
    PGresult* res = executeQuery(sql);
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return nullptr;
    }
    
    auto user = createUserFromRow(res, 0);
    PQclear(res);
    return user;
}

std::shared_ptr<User> Database::getUserById(int userId) {
    std::stringstream sql;
    sql << "SELECT id, username, password, role, balance FROM users WHERE id = " << userId;
    
    PGresult* res = executeQuery(sql.str());
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return nullptr;
    }
    
    auto user = createUserFromRow(res, 0);
    PQclear(res);
    return user;
}

bool Database::updateUserBalance(int userId, float newBalance) {
    std::stringstream sql;
    sql << "UPDATE users SET balance = " << newBalance
        << " WHERE id = " << userId;
    
    return executeSQL(sql.str());
}

bool Database::saveUserBalance(int userId, float newBalance) {
    return updateUserBalance(userId, newBalance);
}

bool Database::updateBalance(int userId, float amount) {
    auto user = getUserById(userId);
    if (!user) {
        std::cerr << "User not found: " << userId << std::endl;
        return false;
    }
    
    float newBalance = user->getBalance() + amount;
    return updateUserBalance(userId, newBalance);
}

bool Database::makePurchase(int userId, float amount) {
    auto user = getUserById(userId);
    if (!user) {
        std::cerr << "User not found: " << userId << std::endl;
        return false;
    }
    
    if (user->getBalance() < amount) {
        std::cerr << "Insufficient funds! Balance: $" << user->getBalance()
                 << ", Required: $" << amount << std::endl;
        return false;
    }
    
    float newBalance = user->getBalance() - amount;
    return updateUserBalance(userId, newBalance);
}

bool Database::addFunds(int userId, float amount) {
    if (amount <= 0) {
        std::cerr << "Amount must be positive: $" << amount << std::endl;
        return false;
    }
    
    return updateBalance(userId, amount);
}

std::vector<std::shared_ptr<Product>> Database::getAllProducts() {
    std::vector<std::shared_ptr<Product>> productList;
    std::string sql = "SELECT id, name, category, price, quantity, size, color FROM products ORDER BY id";
    
    PGresult* res = executeQuery(sql);
    if (!res) return productList;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        auto product = createProductFromRow(res, i);
        if (product) {
            productList.push_back(product);
        }
    }
    
    PQclear(res);
    return productList;
}

std::vector<std::shared_ptr<Product>> Database::getProductsByCategory(Product::Category category) {
    std::vector<std::shared_ptr<Product>> productList;
    std::stringstream sql;
    sql << "SELECT id, name, category, price, quantity, size, color FROM products WHERE category = "
        << static_cast<int>(category) << " ORDER BY id";
    
    PGresult* res = executeQuery(sql.str());
    if (!res) return productList;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        auto product = createProductFromRow(res, i);
        if (product) {
            productList.push_back(product);
        }
    }
    
    PQclear(res);
    return productList;
}

std::shared_ptr<Product> Database::getProductById(int productId) {
    std::stringstream sql;
    sql << "SELECT id, name, category, price, quantity, size, color FROM products WHERE id = " << productId;
    
    PGresult* res = executeQuery(sql.str());
    if (!res || PQntuples(res) == 0) {
        if (res) PQclear(res);
        return nullptr;
    }
    
    auto product = createProductFromRow(res, 0);
    PQclear(res);
    return product;
}

bool Database::updateProductQuantity(int productId, int newQuantity) {
    std::stringstream sql;
    sql << "UPDATE products SET quantity = " << newQuantity
        << " WHERE id = " << productId;
    
    return executeSQL(sql.str());
}

bool Database::addProduct(const Product& product) {
    std::stringstream sql;
    sql << "INSERT INTO products (name, category, price, quantity, size, color) VALUES ('"
        << escapeString(product.getName()) << "', "
        << static_cast<int>(product.getCategory()) << ", "
        << product.getPrice() << ", "
        << product.getQuantity() << ", '"
        << escapeString(product.getSize()) << "', '"
        << escapeString(product.getColor()) << "')";
    
    bool success = executeSQL(sql.str());
    if (success) {
        std::cout << "Product added: " << product.getName() << std::endl;
    }
    return success;
}

bool Database::updateProduct(const Product& updatedProduct) {
    std::stringstream sql;
    sql << "UPDATE products SET "
        << "name = '" << escapeString(updatedProduct.getName()) << "', "
        << "category = " << static_cast<int>(updatedProduct.getCategory()) << ", "
        << "price = " << updatedProduct.getPrice() << ", "
        << "quantity = " << updatedProduct.getQuantity() << ", "
        << "size = '" << escapeString(updatedProduct.getSize()) << "', "
        << "color = '" << escapeString(updatedProduct.getColor()) << "' "
        << "WHERE id = " << updatedProduct.getId();
    
    return executeSQL(sql.str());
}


bool Database::deleteProduct(int productId) {
    std::stringstream sql;
    sql << "UPDATE products SET quantity = 0, name = CONCAT(name, ' [DELETED]') WHERE id = " << productId;
    
    bool success = executeSQL(sql.str());
    if (success) {
        std::cout << "✅ Product ID " << productId << " marked as deleted (quantity set to 0)" << std::endl;
    }
    
    return success;
    
    
}

void Database::refreshUserBalance(std::shared_ptr<User>& user) {
    if (!user || !connection) return;
    
    std::stringstream sql;
    sql << "SELECT balance FROM users WHERE id = " << user->getId();
    
    PGresult* res = executeQuery(sql.str());
    if (res && PQntuples(res) > 0) {
        const char* balanceStr = PQgetvalue(res, 0, 0);
        if (balanceStr) {
            float currentBalance = std::atof(balanceStr);
            user->setBalance(currentBalance);
            std::cout << "Refreshed balance for user " << user->getUsername()
                     << ": $" << currentBalance << std::endl;
        }
        PQclear(res);
    }
}

bool Database::createOrderDirect(int userId, const std::vector<std::pair<int, int>>& items) {
    std::cout << "\n=== CREATING ORDER (DIRECT) ===" << std::endl;
    
    if (items.empty()) {
        std::cout << "❌ No items in cart!" << std::endl;
        return false;
    }
    
    // 1. Получаем информацию о пользователе и админе
    std::stringstream userInfoSql;
    userInfoSql << "SELECT id, username, balance FROM users WHERE id = " << userId;
    PGresult* userInfoRes = PQexec(connection, userInfoSql.str().c_str());
    
    if (!userInfoRes || PQntuples(userInfoRes) == 0) {
        std::cout << "❌ User not found!" << std::endl;
        if (userInfoRes) PQclear(userInfoRes);
        return false;
    }
    
    const char* username = PQgetvalue(userInfoRes, 0, 1);
    float userBalance = atof(PQgetvalue(userInfoRes, 0, 2));
    PQclear(userInfoRes);
    
    // 2. Находим админа (первый пользователь с ролью SELLER)
    std::string adminSql = "SELECT id, username, balance FROM users WHERE role = "
                          + std::to_string(static_cast<int>(User::Role::SELLER))
                          + " ORDER BY id LIMIT 1";
    
    PGresult* adminRes = PQexec(connection, adminSql.c_str());
    if (!adminRes || PQntuples(adminRes) == 0) {
        std::cout << "❌ No seller (admin) found!" << std::endl;
        if (adminRes) PQclear(adminRes);
        return false;
    }
    
    int adminId = atoi(PQgetvalue(adminRes, 0, 0));
    const char* adminUsername = PQgetvalue(adminRes, 0, 1);
    float adminBalance = atof(PQgetvalue(adminRes, 0, 2));
    PQclear(adminRes);
    
    std::cout << "👤 Customer: " << username
              << " (ID: " << userId << "), Balance: $" << userBalance << std::endl;
    std::cout << "👔 Seller: " << adminUsername
              << " (ID: " << adminId << "), Balance: $" << adminBalance << std::endl;
    
    // 3. Проверяем наличие товаров и рассчитываем сумму
    float totalAmount = 0;
    std::vector<std::tuple<int, std::string, float, int>> itemDetails; // id, name, price, requested_qty
    
    for (const auto& item : items) {
        int productId = item.first;
        int requestedQty = item.second;
        
        std::stringstream productSql;
        productSql << "SELECT name, price, quantity FROM products WHERE id = " << productId;
        PGresult* productRes = PQexec(connection, productSql.str().c_str());
        
        if (!productRes || PQntuples(productRes) == 0) {
            std::cout << "❌ Product not found: ID " << productId << std::endl;
            if (productRes) PQclear(productRes);
            return false;
        }
        
        const char* productName = PQgetvalue(productRes, 0, 0);
        float price = atof(PQgetvalue(productRes, 0, 1));
        int availableQty = atoi(PQgetvalue(productRes, 0, 2));
        PQclear(productRes);
        
        if (availableQty < requestedQty) {
            std::cout << "❌ Not enough stock for '" << productName
                     << "' (available: " << availableQty
                     << ", requested: " << requestedQty << ")" << std::endl;
            return false;
        }
        
        float itemTotal = price * requestedQty;
        totalAmount += itemTotal;
        itemDetails.push_back(std::make_tuple(productId, std::string(productName), price, requestedQty));
        
        std::cout << "🛒 " << productName
                 << " x" << requestedQty << " @ $" << price
                 << " = $" << itemTotal << std::endl;
    }
    
    std::cout << "💰 Total amount: $" << std::fixed << std::setprecision(2) << totalAmount << std::endl;
    
    // 4. Проверяем баланс пользователя
    if (userBalance < totalAmount) {
        std::cout << "❌ Insufficient funds! Balance: $" << userBalance
                 << ", Required: $" << totalAmount << std::endl;
        return false;
    }
    
    // 5. Выполняем всю операцию в одной транзакции
    PGresult* beginRes = PQexec(connection, "BEGIN");
    if (PQresultStatus(beginRes) != PGRES_COMMAND_OK) {
        std::cout << "❌ Failed to start transaction: " << PQerrorMessage(connection) << std::endl;
        PQclear(beginRes);
        return false;
    }
    PQclear(beginRes);
    
    bool success = false;
    
    try {
        // 6. Создаем заказ с использованием RETURNING для получения ID
        std::stringstream orderSql;
        orderSql << "INSERT INTO orders (user_id, total_amount) VALUES ("
                 << userId << ", " << totalAmount << ") RETURNING id";
        
        PGresult* orderRes = PQexec(connection, orderSql.str().c_str());
        if (PQresultStatus(orderRes) != PGRES_TUPLES_OK) {
            throw std::runtime_error("Failed to create order: " + std::string(PQerrorMessage(connection)));
        }
        
        int orderId = atoi(PQgetvalue(orderRes, 0, 0));
        PQclear(orderRes);
        
        std::cout << "📝 Order created! ID: " << orderId << std::endl;
        
        // 7. Добавляем товары в заказ и обновляем остатки
        std::cout << "\n🛍️ Processing items..." << std::endl;
        
        for (const auto& item : itemDetails) {
            int productId = std::get<0>(item);
            const std::string& productName = std::get<1>(item);
            float price = std::get<2>(item);
            int requestedQty = std::get<3>(item);
            
            // Добавляем товар в заказ
            std::stringstream itemSql;
            itemSql << "INSERT INTO order_items (order_id, product_id, quantity, price_at_time) VALUES ("
                   << orderId << ", " << productId << ", " << requestedQty << ", " << price << ")";
            
            PGresult* itemRes = PQexec(connection, itemSql.str().c_str());
            if (PQresultStatus(itemRes) != PGRES_COMMAND_OK) {
                throw std::runtime_error("Failed to add item " + productName);
            }
            PQclear(itemRes);
            
            // Обновляем количество товара
            std::stringstream updateSql;
            updateSql << "UPDATE products SET quantity = quantity - " << requestedQty
                     << " WHERE id = " << productId;
            
            PGresult* updateRes = PQexec(connection, updateSql.str().c_str());
            if (PQresultStatus(updateRes) != PGRES_COMMAND_OK) {
                throw std::runtime_error("Failed to update quantity for " + productName);
            }
            PQclear(updateRes);
            
            std::cout << "✅ " << productName << " x" << requestedQty
                     << " @ $" << price << std::endl;
        }
        
        // 8. ОБНОВЛЯЕМ БАЛАНСЫ: снимаем у покупателя, добавляем продавцу
        std::cout << "\n💸 Updating balances..." << std::endl;
        
        // 8.1 Снимаем у покупателя
        std::stringstream updateUserSql;
        updateUserSql << "UPDATE users SET balance = balance - " << totalAmount
                     << " WHERE id = " << userId << " RETURNING balance";
        
        PGresult* userUpdateRes = PQexec(connection, updateUserSql.str().c_str());
        if (PQresultStatus(userUpdateRes) != PGRES_TUPLES_OK) {
            throw std::runtime_error("Failed to update user balance");
        }
        float newUserBalance = atof(PQgetvalue(userUpdateRes, 0, 0));
        PQclear(userUpdateRes);
        
        std::cout << "   " << username << ": $" << userBalance << " → $" << newUserBalance
                 << " (-$" << totalAmount << ")" << std::endl;
        
        // 8.2 Добавляем продавцу
        std::stringstream updateAdminSql;
        updateAdminSql << "UPDATE users SET balance = balance + " << totalAmount
                      << " WHERE id = " << adminId << " RETURNING balance";
        
        PGresult* adminUpdateRes = PQexec(connection, updateAdminSql.str().c_str());
        if (PQresultStatus(adminUpdateRes) != PGRES_TUPLES_OK) {
            throw std::runtime_error("Failed to update seller balance");
        }
        float newAdminBalance = atof(PQgetvalue(adminUpdateRes, 0, 0));
        PQclear(adminUpdateRes);
        
        std::cout << "   " << adminUsername << ": $" << adminBalance << " → $" << newAdminBalance
                 << " (+$" << totalAmount << ")" << std::endl;
        
        // 9. Фиксируем транзакцию
        PGresult* commitRes = PQexec(connection, "COMMIT");
        if (PQresultStatus(commitRes) != PGRES_COMMAND_OK) {
            throw std::runtime_error("Failed to commit transaction");
        }
        PQclear(commitRes);
        
        success = true;
        
        std::cout << "\n🔍 FINAL VERIFICATION:" << std::endl;
        std::cout << "💰 Total transferred: $" << totalAmount << std::endl;
        std::cout << "📦 Order ID: " << orderId << std::endl;
        std::cout << "🛍️ Items processed: " << items.size() << std::endl;
        std::cout << "\n🎉 PURCHASE COMPLETED SUCCESSFULLY!" << std::endl;
        
    } catch (const std::exception& e) {
        // Откатываем транзакцию при ошибке
        std::cout << "⚠️ Error: " << e.what() << std::endl;
        std::cout << "Rolling back transaction..." << std::endl;
        
        PGresult* rollbackRes = PQexec(connection, "ROLLBACK");
        PQclear(rollbackRes);
        
        std::cout << "❌ Transaction rolled back. No changes were made." << std::endl;
        success = false;
    }
    
    return success;
}

std::vector<std::pair<int, std::string>> Database::getUserOrders(int userId) {
    std::vector<std::pair<int, std::string>> orders;
    std::stringstream sql;
    sql << "SELECT id, order_date FROM orders WHERE user_id = " << userId << " ORDER BY order_date DESC";
    
    PGresult* res = executeQuery(sql.str());
    if (!res) return orders;
    
    int rows = PQntuples(res);
    for (int i = 0; i < rows; i++) {
        const char* idStr = PQgetvalue(res, i, 0);
        const char* dateStr = PQgetvalue(res, i, 1);
        
        if (idStr && dateStr) {
            int orderId = std::atoi(idStr);
            orders.push_back({orderId, dateStr});
        }
    }
    
    PQclear(res);
    return orders;
}

// Вспомогательные методы

std::shared_ptr<User> Database::createUserFromRow(PGresult* res, int row) {
    try {
        const char* idStr = PQgetvalue(res, row, 0);
        const char* username = PQgetvalue(res, row, 1);
        const char* password = PQgetvalue(res, row, 2);
        const char* roleStr = PQgetvalue(res, row, 3);
        const char* balanceStr = PQgetvalue(res, row, 4);
        
        if (!idStr || !username || !password || !roleStr || !balanceStr) {
            return nullptr;
        }
        
        int id = std::atoi(idStr);
        User::Role role = static_cast<User::Role>(std::atoi(roleStr));
        float balance = std::atof(balanceStr);
        
        return std::make_shared<User>(id, username, password, role, balance);
    } catch (...) {
        return nullptr;
    }
}

std::shared_ptr<Product> Database::createProductFromRow(PGresult* res, int row) {
    try {
        const char* idStr = PQgetvalue(res, row, 0);
        const char* name = PQgetvalue(res, row, 1);
        const char* categoryStr = PQgetvalue(res, row, 2);
        const char* priceStr = PQgetvalue(res, row, 3);
        const char* quantityStr = PQgetvalue(res, row, 4);
        const char* size = PQgetvalue(res, row, 5);
        const char* color = PQgetvalue(res, row, 6);
        
        if (!idStr || !name || !categoryStr || !priceStr || !quantityStr) {
            return nullptr;
        }
        
        int id = std::atoi(idStr);
        Product::Category category = static_cast<Product::Category>(std::atoi(categoryStr));
        float price = std::atof(priceStr);
        int quantity = std::atoi(quantityStr);
        
        std::string sizeStr = size ? size : "";
        std::string colorStr = color ? color : "";
        
        return std::make_shared<Product>(id, name, category, price, quantity, sizeStr, colorStr);
    } catch (...) {
        return nullptr;
    }
}

bool Database::executeSQL(const std::string& sql) {
    if (!connection) {
        std::cerr << "No database connection" << std::endl;
        return false;
    }
    
    PGresult* res = PQexec(connection, sql.c_str());
    ExecStatusType status = PQresultStatus(res);
    
    if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK) {
        std::cerr << "SQL error: " << PQerrorMessage(connection) << std::endl;
        std::cerr << "Failed SQL: " << sql << std::endl;
        PQclear(res);
        return false;
    }
    
    PQclear(res);
    return true;
}

PGresult* Database::executeQuery(const std::string& sql) {
    if (!connection) {
        std::cerr << "No database connection" << std::endl;
        return nullptr;
    }
    
    PGresult* res = PQexec(connection, sql.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query error: " << PQerrorMessage(connection) << std::endl;
        PQclear(res);
        return nullptr;
    }
    
    return res;
}

std::string Database::escapeString(const std::string& str) {
    if (!connection) return str;
    
    char* escaped = new char[str.length() * 2 + 1];
    int error;
    PQescapeStringConn(connection, escaped, str.c_str(), str.length(), &error);
    
    if (error) {
        delete[] escaped;
        return str;
    }
    
    std::string result(escaped);
    delete[] escaped;
    
    return result;
}

