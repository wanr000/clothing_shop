#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "user.hpp"
#include "product.hpp"
#include <memory>
#include <vector>
#include <string>

typedef struct pg_conn PGconn;
typedef struct pg_result PGresult;

// Структура для отзыва
struct Feedback {
    int id;
    int userId;
    std::string username;
    std::string feedback;
    int rating;
    std::string createdAt;
    
    Feedback() : id(0), userId(0), rating(5) {}
    
    Feedback(int id, int userId, const std::string& username,
             const std::string& feedback, int rating, const std::string& createdAt)
        : id(id), userId(userId), username(username),
          feedback(feedback), rating(rating), createdAt(createdAt) {}
};

class Database {
private:
    PGconn* connection;
    std::string connectionString;
    
    void createTables();
    
public:
    Database(const std::string& connStr = "host=localhost port=5432 dbname=autocar user=postgres password=");
    ~Database();
    
    bool initialize();
    bool connect();
    void disconnect();
    
    // Пользователи
    std::shared_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& password, User::Role role, float balance);
    std::shared_ptr<User> getUserByUsername(const std::string& username);
    std::shared_ptr<User> getUserById(int userId);
    bool updateUserBalance(int userId, float newBalance);
    bool saveUserBalance(int userId, float newBalance); // Основной метод для сохранения
    
    // Товары
    std::vector<std::shared_ptr<Product>> getAllProducts();
    std::vector<std::shared_ptr<Product>> getProductsByCategory(Product::Category category);
    std::shared_ptr<Product> getProductById(int productId);
    bool updateProductQuantity(int productId, int newQuantity);
    bool addProduct(const Product& product);
    bool updateProduct(const Product& updatedProduct);
    bool deleteProduct(int productId);
    
    // Заказы и баланс
    bool createOrderDirect(int userId, const std::vector<std::pair<int, int>>& items);
    bool createOrder(int userId, const std::vector<std::pair<int, int>>& items);
    bool updateBalance(int userId, float amount); // Для пополнения/списания
    bool makePurchase(int userId, float amount); // Для покупки
    bool addFunds(int userId, float amount); // Для пополнения
    std::vector<std::pair<int, std::string>> getUserOrders(int userId);
    void refreshUserBalance(std::shared_ptr<User>& user);
    
    // Отзывы
    bool addFeedback(int userId, const std::string& feedback, int rating);
    std::vector<Feedback> getRecentFeedbacks(int limit = 10);
    std::vector<Feedback> getUserFeedbacks(int userId);
    std::vector<Feedback> getProductFeedbacks(int productId);
    float getAverageRating();
    int getFeedbackCount();
    bool deleteFeedback(int feedbackId);
    bool updateFeedback(int feedbackId, const std::string& newFeedback, int newRating);
    
private:
    // Вспомогательные методы
    std::shared_ptr<User> createUserFromRow(PGresult* res, int row);
    std::shared_ptr<Product> createProductFromRow(PGresult* res, int row);
    Feedback createFeedbackFromRow(PGresult* res, int row);
    
    // Исполнение SQL
    bool executeSQL(const std::string& sql);
    PGresult* executeQuery(const std::string& sql);
    
    // Экранирование строк
    std::string escapeString(const std::string& str);
    
    // Тестовые данные
    void createSampleData();
};

#endif // DATABASE_HPP
