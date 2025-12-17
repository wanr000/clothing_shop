#include "storeapp.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <cstdlib>

StoreApp::StoreApp()
    : window(sf::VideoMode(1200, 800), "Auto House")
    , currentScreen(Screen::AUTH)
    , previousScreen(Screen::AUTH)
    , productEditScreen(Screen::ADMIN)
    , selectedProductIndex(-1)
    , selectedAdminProductIndex(-1)
    , isEditingProduct(false)
    , editingProductId(-1)
    , typingUsername(false)
    , typingPassword(false)
    , typingRegUsername(false)
    , typingRegPassword(false)
    , typingRegConfirm(false)
    , typingName(false)
    , typingPrice(false)
    , typingQuantity(false)
    , typingSize(false)
    , typingColor(false)
    , typingCategory(false)
    , selectedCategoryIndex(0)
    , typingFeedback(false)
    , feedbackRating(5) {
    
    // Загрузка шрифта
    #ifdef __APPLE__
        if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            if (!font.loadFromFile("/System/Library/Fonts/Arial.ttf")) {
                std::cout << "Using default font" << std::endl;
            }
        }
    #else
        // Для Windows/Linux
        if (!font.loadFromFile("arial.ttf")) {
            std::cout << "Using default font" << std::endl;
        }
    #endif
    
    window.setFramerateLimit(60);
    database.initialize();
    products = database.getAllProducts();
    
    // Инициализация категорий
    categoryNames = {"Superar", "Hypercar", "Family Car", "Race Car", "Collector's Car", "just car"};
    
    setupUI();
}

void StoreApp::setupUI() {
    // Настройка цветов
    sf::Color backgroundColor(30, 30, 50);
    sf::Color panelColor(40, 40, 60);
    sf::Color buttonColor(0, 100, 200);
    sf::Color buttonHoverColor(0, 150, 255);
    
    // ===== АВТОРИЗАЦИЯ =====
    authTitle.setFont(font);
    authTitle.setString("Auto House");
    authTitle.setCharacterSize(36);
    authTitle.setColor(sf::Color::White);
    authTitle.setPosition(400, 50);
    
    usernameLabel.setFont(font);
    usernameLabel.setString("Username:");
    usernameLabel.setCharacterSize(24);
    usernameLabel.setColor(sf::Color::White);
    usernameLabel.setPosition(400, 150);
    
    passwordLabel.setFont(font);
    passwordLabel.setString("Password:");
    passwordLabel.setCharacterSize(24);
    passwordLabel.setColor(sf::Color::White);
    passwordLabel.setPosition(400, 250);
    
    usernameBox.setSize(sf::Vector2f(300, 40));
    usernameBox.setPosition(400, 180);
    usernameBox.setFillColor(panelColor);
    usernameBox.setOutlineColor(sf::Color::White);
    usernameBox.setOutlineThickness(2);
    
    passwordBox.setSize(sf::Vector2f(300, 40));
    passwordBox.setPosition(400, 280);
    passwordBox.setFillColor(panelColor);
    passwordBox.setOutlineColor(sf::Color::White);
    passwordBox.setOutlineThickness(2);
    
    usernameText.setFont(font);
    usernameText.setString("");
    usernameText.setCharacterSize(20);
    usernameText.setColor(sf::Color::White);
    usernameText.setPosition(410, 185);
    
    passwordText.setFont(font);
    passwordText.setString("");
    passwordText.setCharacterSize(20);
    passwordText.setColor(sf::Color::White);
    passwordText.setPosition(410, 285);
    
    // Кнопка входа покупателя
    loginButton.setSize(sf::Vector2f(200, 50));
    loginButton.setPosition(400, 350);
    loginButton.setFillColor(buttonColor);
    
    loginButtonText.setFont(font);
    loginButtonText.setString("Login as Customer");
    loginButtonText.setCharacterSize(20);
    loginButtonText.setColor(sf::Color::White);
    loginButtonText.setPosition(420, 365);
    
    // Кнопка регистрации
    registerButton.setSize(sf::Vector2f(200, 50));
    registerButton.setPosition(400, 420);
    registerButton.setFillColor(sf::Color(0, 150, 0));
    
    registerButtonText.setFont(font);
    registerButtonText.setString("Register");
    registerButtonText.setCharacterSize(20);
    registerButtonText.setColor(sf::Color::White);
    registerButtonText.setPosition(460, 435);
    
    // Кнопка входа админа
    adminButton.setSize(sf::Vector2f(200, 50));
    adminButton.setPosition(400, 490);
    adminButton.setFillColor(sf::Color(200, 100, 0));
    
    adminButtonText.setFont(font);
    adminButtonText.setString("Admin Login");
    adminButtonText.setCharacterSize(20);
    adminButtonText.setColor(sf::Color::White);
    adminButtonText.setPosition(430, 505);
    
    messageText.setFont(font);
    messageText.setString("");
    messageText.setCharacterSize(18);
    messageText.setColor(sf::Color::Red);
    messageText.setPosition(400, 560);
    
    // ===== РЕГИСТРАЦИЯ =====
    registerTitle.setFont(font);
    registerTitle.setString("Create New Account");
    registerTitle.setCharacterSize(36);
    registerTitle.setColor(sf::Color::White);
    registerTitle.setPosition(400, 50);
    
    regUsernameLabel.setFont(font);
    regUsernameLabel.setString("Username:");
    regUsernameLabel.setCharacterSize(24);
    regUsernameLabel.setColor(sf::Color::White);
    regUsernameLabel.setPosition(400, 150);
    
    regPasswordLabel.setFont(font);
    regPasswordLabel.setString("Password:");
    regPasswordLabel.setCharacterSize(24);
    regPasswordLabel.setColor(sf::Color::White);
    regPasswordLabel.setPosition(400, 250);
    
    regConfirmLabel.setFont(font);
    regConfirmLabel.setString("Confirm Password:");
    regConfirmLabel.setCharacterSize(24);
    regConfirmLabel.setColor(sf::Color::White);
    regConfirmLabel.setPosition(400, 350);
    
    regUsernameBox.setSize(sf::Vector2f(300, 40));
    regUsernameBox.setPosition(400, 180);
    regUsernameBox.setFillColor(panelColor);
    regUsernameBox.setOutlineColor(sf::Color::White);
    regUsernameBox.setOutlineThickness(2);
    
    regPasswordBox.setSize(sf::Vector2f(300, 40));
    regPasswordBox.setPosition(400, 280);
    regPasswordBox.setFillColor(panelColor);
    regPasswordBox.setOutlineColor(sf::Color::White);
    regPasswordBox.setOutlineThickness(2);
    
    regConfirmBox.setSize(sf::Vector2f(300, 40));
    regConfirmBox.setPosition(400, 380);
    regConfirmBox.setFillColor(panelColor);
    regConfirmBox.setOutlineColor(sf::Color::White);
    regConfirmBox.setOutlineThickness(2);
    
    regUsernameText.setFont(font);
    regUsernameText.setString("");
    regUsernameText.setCharacterSize(20);
    regUsernameText.setColor(sf::Color::White);
    regUsernameText.setPosition(410, 185);
    
    regPasswordText.setFont(font);
    regPasswordText.setString("");
    regPasswordText.setCharacterSize(20);
    regPasswordText.setColor(sf::Color::White);
    regPasswordText.setPosition(410, 285);
    
    regConfirmText.setFont(font);
    regConfirmText.setString("");
    regConfirmText.setCharacterSize(20);
    regConfirmText.setColor(sf::Color::White);
    regConfirmText.setPosition(410, 385);
    
    createAccountButton.setSize(sf::Vector2f(200, 50));
    createAccountButton.setPosition(400, 450);
    createAccountButton.setFillColor(sf::Color(0, 150, 0));
    
    createAccountText.setFont(font);
    createAccountText.setString("Create Account");
    createAccountText.setCharacterSize(20);
    createAccountText.setColor(sf::Color::White);
    createAccountText.setPosition(425, 465);
    
    backButton.setSize(sf::Vector2f(200, 50));
    backButton.setPosition(400, 520);
    backButton.setFillColor(sf::Color(150, 150, 150));
    
    backButtonText.setFont(font);
    backButtonText.setString("Back to Login");
    backButtonText.setCharacterSize(20);
    backButtonText.setColor(sf::Color::White);
    backButtonText.setPosition(430, 535);
    
    // ===== ПОКУПАТЕЛЬ (ТОВАРЫ) =====
    customerTitle.setFont(font);
    customerTitle.setString("Cars");
    customerTitle.setCharacterSize(36);
    customerTitle.setColor(sf::Color::White);
    customerTitle.setPosition(50, 20);
    
    balanceLabel.setFont(font);
    balanceLabel.setCharacterSize(24);
    balanceLabel.setColor(sf::Color::Green);
    balanceLabel.setPosition(50, 70);
    
    productsBackground.setSize(sf::Vector2f(1100, 500));
    productsBackground.setPosition(50, 120);
    productsBackground.setFillColor(panelColor);
    productsBackground.setOutlineColor(sf::Color::White);
    productsBackground.setOutlineThickness(2);
    
    viewCartButton.setSize(sf::Vector2f(200, 50));
    viewCartButton.setPosition(50, 650);
    viewCartButton.setFillColor(buttonColor);
    
    viewCartText.setFont(font);
    viewCartText.setString("View Cart");
    viewCartText.setCharacterSize(20);
    viewCartText.setColor(sf::Color::White);
    viewCartText.setPosition(100, 665);
    
    logoutButton.setSize(sf::Vector2f(200, 50));
    logoutButton.setPosition(300, 650);
    logoutButton.setFillColor(sf::Color(200, 50, 50));
    
    logoutText.setFont(font);
    logoutText.setString("Logout");
    logoutText.setCharacterSize(20);
    logoutText.setColor(sf::Color::White);
    logoutText.setPosition(370, 665);
    
    // ===== АДМИН =====
    adminTitle.setFont(font);
    adminTitle.setString("Admin Panel - Cars Management");
    adminTitle.setCharacterSize(36);
    adminTitle.setColor(sf::Color::White);
    adminTitle.setPosition(50, 20);
    
    adminBalanceLabel.setFont(font);
    adminBalanceLabel.setCharacterSize(24);
    adminBalanceLabel.setColor(sf::Color::Green);
    adminBalanceLabel.setPosition(50, 70);
    
    adminProductsBackground.setSize(sf::Vector2f(1100, 450));
    adminProductsBackground.setPosition(50, 120);
    adminProductsBackground.setFillColor(panelColor);
    adminProductsBackground.setOutlineColor(sf::Color::White);
    adminProductsBackground.setOutlineThickness(2);
    
    addProductButton.setSize(sf::Vector2f(200, 50));
    addProductButton.setPosition(50, 600);
    addProductButton.setFillColor(sf::Color(0, 150, 0));
    
    addProductText.setFont(font);
    addProductText.setString("Add Car");
    addProductText.setCharacterSize(20);
    addProductText.setColor(sf::Color::White);
    addProductText.setPosition(80, 615);
    
    editProductButton.setSize(sf::Vector2f(200, 50));
    editProductButton.setPosition(300, 600);
    editProductButton.setFillColor(sf::Color(0, 100, 200));
    
    editProductText.setFont(font);
    editProductText.setString("Edit Car Info");
    editProductText.setCharacterSize(20);
    editProductText.setColor(sf::Color::White);
    editProductText.setPosition(330, 615);
    
    deleteProductButton.setSize(sf::Vector2f(200, 50));
    deleteProductButton.setPosition(550, 600);
    deleteProductButton.setFillColor(sf::Color(200, 50, 50));
    
    deleteProductText.setFont(font);
    deleteProductText.setString("Delete Car Info");
    deleteProductText.setCharacterSize(20);
    deleteProductText.setColor(sf::Color::White);
    deleteProductText.setPosition(570, 615);
    
    adminLogoutButton.setSize(sf::Vector2f(200, 50));
    adminLogoutButton.setPosition(800, 600);
    adminLogoutButton.setFillColor(sf::Color(150, 150, 150));
    
    adminLogoutText.setFont(font);
    adminLogoutText.setString("Logout");
    adminLogoutText.setCharacterSize(20);
    adminLogoutText.setColor(sf::Color::White);
    adminLogoutText.setPosition(870, 615);
    
    // ===== КОРЗИНА =====
    cartTitle.setFont(font);
    cartTitle.setString("Shopping Cart");
    cartTitle.setCharacterSize(36);
    cartTitle.setColor(sf::Color::White);
    cartTitle.setPosition(50, 20);
    
    cartBalanceLabel.setFont(font);
    cartBalanceLabel.setCharacterSize(24);
    cartBalanceLabel.setColor(sf::Color::Green);
    cartBalanceLabel.setPosition(50, 70);
    
    cartTotalLabel.setFont(font);
    cartTotalLabel.setCharacterSize(28);
    cartTotalLabel.setColor(sf::Color::Yellow);
    cartTotalLabel.setPosition(50, 110);
    
    cartItemsBackground.setSize(sf::Vector2f(1100, 400));
    cartItemsBackground.setPosition(50, 160);
    cartItemsBackground.setFillColor(panelColor);
    cartItemsBackground.setOutlineColor(sf::Color::White);
    cartItemsBackground.setOutlineThickness(2);
    
    checkoutButton.setSize(sf::Vector2f(200, 50));
    checkoutButton.setPosition(50, 600);
    checkoutButton.setFillColor(sf::Color(0, 150, 0));
    
    checkoutText.setFont(font);
    checkoutText.setString("Checkout");
    checkoutText.setCharacterSize(20);
    checkoutText.setColor(sf::Color::White);
    checkoutText.setPosition(100, 615);
    
    continueShoppingButton.setSize(sf::Vector2f(200, 50));
    continueShoppingButton.setPosition(300, 600);
    continueShoppingButton.setFillColor(buttonColor);
    
    continueShoppingText.setFont(font);
    continueShoppingText.setString("Continue Shopping");
    continueShoppingText.setCharacterSize(20);
    continueShoppingText.setColor(sf::Color::White);
    continueShoppingText.setPosition(310, 615);
    
    clearCartButton.setSize(sf::Vector2f(200, 50));
    clearCartButton.setPosition(550, 600);
    clearCartButton.setFillColor(sf::Color(200, 50, 50));
    
    clearCartText.setFont(font);
    clearCartText.setString("Clear Cart");
    clearCartText.setCharacterSize(20);
    clearCartText.setColor(sf::Color::White);
    clearCartText.setPosition(600, 615);
    
    // ===== ЧЕК =====
    receiptTitle.setFont(font);
    receiptTitle.setString("Purchase Receipt");
    receiptTitle.setCharacterSize(36);
    receiptTitle.setColor(sf::Color::White);
    receiptTitle.setPosition(400, 50);
    
    receiptBackground.setSize(sf::Vector2f(800, 500));
    receiptBackground.setPosition(200, 120);
    receiptBackground.setFillColor(panelColor);
    receiptBackground.setOutlineColor(sf::Color::White);
    receiptBackground.setOutlineThickness(2);
    
    receiptDetails.setFont(font);
    receiptDetails.setCharacterSize(20);
    receiptDetails.setColor(sf::Color::White);
    receiptDetails.setPosition(220, 140);
    
    receiptOkButton.setSize(sf::Vector2f(200, 50));
    receiptOkButton.setPosition(500, 650);
    receiptOkButton.setFillColor(buttonColor);
    
    receiptOkText.setFont(font);
    receiptOkText.setString("Leave Feedback");
    receiptOkText.setCharacterSize(20);
    receiptOkText.setColor(sf::Color::White);
    receiptOkText.setPosition(520, 665);
    
    // ===== ОТЗЫВЫ =====
    feedbackTitle.setFont(font);
    feedbackTitle.setString("Leave Your Feedback");
    feedbackTitle.setCharacterSize(36);
    feedbackTitle.setColor(sf::Color::White);
    feedbackTitle.setPosition(400, 50);
    
    ratingLabel.setFont(font);
    ratingLabel.setString("Rating:");
    ratingLabel.setCharacterSize(24);
    ratingLabel.setColor(sf::Color::White);
    ratingLabel.setPosition(400, 120);
    
    feedbackLabel.setFont(font);
    feedbackLabel.setString("Your Feedback:");
    feedbackLabel.setCharacterSize(24);
    feedbackLabel.setColor(sf::Color::White);
    feedbackLabel.setPosition(400, 200);
    
    feedbackBox.setSize(sf::Vector2f(600, 200));
    feedbackBox.setPosition(400, 240);
    feedbackBox.setFillColor(panelColor);
    feedbackBox.setOutlineColor(sf::Color::White);
    feedbackBox.setOutlineThickness(2);
    
    feedbackText.setFont(font);
    feedbackText.setString("");
    feedbackText.setCharacterSize(20);
    feedbackText.setColor(sf::Color::White);
    feedbackText.setPosition(410, 250);
    
    ratingStars.setFont(font);
    ratingStars.setString("");
    ratingStars.setCharacterSize(40);
    ratingStars.setColor(sf::Color::Yellow);
    ratingStars.setPosition(500, 110);
    
    submitFeedbackButton.setSize(sf::Vector2f(200, 50));
    submitFeedbackButton.setPosition(400, 500);
    submitFeedbackButton.setFillColor(sf::Color(0, 150, 0));
    
    submitFeedbackText.setFont(font);
    submitFeedbackText.setString("Submit Feedback");
    submitFeedbackText.setCharacterSize(20);
    submitFeedbackText.setColor(sf::Color::White);
    submitFeedbackText.setPosition(410, 515);
    
    skipFeedbackButton.setSize(sf::Vector2f(200, 50));
    skipFeedbackButton.setPosition(650, 500);
    skipFeedbackButton.setFillColor(sf::Color(150, 150, 150));
    
    skipFeedbackText.setFont(font);
    skipFeedbackText.setString("Skip");
    skipFeedbackText.setCharacterSize(20);
    skipFeedbackText.setColor(sf::Color::White);
    skipFeedbackText.setPosition(720, 515);
    
    // ===== ФОРМА ТОВАРА (ДОБАВЛЕНИЕ/РЕДАКТИРОВАНИЕ) =====
    productFormTitle.setFont(font);
    productFormTitle.setString("Car Form");
    productFormTitle.setCharacterSize(36);
    productFormTitle.setColor(sf::Color::White);
    productFormTitle.setPosition(400, 50);
    
    // Название
    nameLabel.setFont(font);
    nameLabel.setString("Car Brand:");
    nameLabel.setCharacterSize(24);
    nameLabel.setColor(sf::Color::White);
    nameLabel.setPosition(400, 120);
    
    nameBox.setSize(sf::Vector2f(300, 40));
    nameBox.setPosition(400, 150);
    nameBox.setFillColor(panelColor);
    nameBox.setOutlineColor(sf::Color::White);
    nameBox.setOutlineThickness(2);
    
    nameText.setFont(font);
    nameText.setString("");
    nameText.setCharacterSize(20);
    nameText.setColor(sf::Color::White);
    nameText.setPosition(410, 155);
    
    // Категория
    categoryLabel.setFont(font);
    categoryLabel.setString("Type:");
    categoryLabel.setCharacterSize(24);
    categoryLabel.setColor(sf::Color::White);
    categoryLabel.setPosition(400, 210);
    
    categoryBox.setSize(sf::Vector2f(300, 40));
    categoryBox.setPosition(400, 240);
    categoryBox.setFillColor(panelColor);
    categoryBox.setOutlineColor(sf::Color::White);
    categoryBox.setOutlineThickness(2);
    
    categoryText.setFont(font);
    categoryText.setString("Hypercar");
    categoryText.setCharacterSize(20);
    categoryText.setColor(sf::Color::White);
    categoryText.setPosition(410, 245);
    
    // Цена
    priceLabel.setFont(font);
    priceLabel.setString("Price ($):");
    priceLabel.setCharacterSize(24);
    priceLabel.setColor(sf::Color::White);
    priceLabel.setPosition(400, 300);
    
    priceBox.setSize(sf::Vector2f(300, 40));
    priceBox.setPosition(400, 330);
    priceBox.setFillColor(panelColor);
    priceBox.setOutlineColor(sf::Color::White);
    priceBox.setOutlineThickness(2);
    
    priceText.setFont(font);
    priceText.setString("");
    priceText.setCharacterSize(20);
    priceText.setColor(sf::Color::White);
    priceText.setPosition(410, 335);
    
    // Количество
    quantityLabel.setFont(font);
    quantityLabel.setString("Quantity:");
    quantityLabel.setCharacterSize(24);
    quantityLabel.setColor(sf::Color::White);
    quantityLabel.setPosition(400, 390);
    
    quantityBox.setSize(sf::Vector2f(300, 40));
    quantityBox.setPosition(400, 420);
    quantityBox.setFillColor(panelColor);
    quantityBox.setOutlineColor(sf::Color::White);
    quantityBox.setOutlineThickness(2);
    
    quantityText.setFont(font);
    quantityText.setString("");
    quantityText.setCharacterSize(20);
    quantityText.setColor(sf::Color::White);
    quantityText.setPosition(410, 425);
    
    // Размер
    sizeLabel.setFont(font);
    sizeLabel.setString("HP:");
    sizeLabel.setCharacterSize(24);
    sizeLabel.setColor(sf::Color::White);
    sizeLabel.setPosition(750, 120);
    
    sizeBox.setSize(sf::Vector2f(300, 40));
    sizeBox.setPosition(750, 150);
    sizeBox.setFillColor(panelColor);
    sizeBox.setOutlineColor(sf::Color::White);
    sizeBox.setOutlineThickness(2);
    
    sizeText.setFont(font);
    sizeText.setString("");
    sizeText.setCharacterSize(20);
    sizeText.setColor(sf::Color::White);
    sizeText.setPosition(760, 155);
    
    // Цвет
    colorLabel.setFont(font);
    colorLabel.setString("Color:");
    colorLabel.setCharacterSize(24);
    colorLabel.setColor(sf::Color::White);
    colorLabel.setPosition(750, 210);
    
    colorBox.setSize(sf::Vector2f(300, 40));
    colorBox.setPosition(750, 240);
    colorBox.setFillColor(panelColor);
    colorBox.setOutlineColor(sf::Color::White);
    colorBox.setOutlineThickness(2);
    
    colorText.setFont(font);
    colorText.setString("");
    colorText.setCharacterSize(20);
    colorText.setColor(sf::Color::White);
    colorText.setPosition(760, 245);
    
    // Кнопки формы
    saveButton.setSize(sf::Vector2f(200, 50));
    saveButton.setPosition(400, 520);
    saveButton.setFillColor(sf::Color(0, 150, 0));
    
    saveButtonText.setFont(font);
    saveButtonText.setString("Save");
    saveButtonText.setCharacterSize(20);
    saveButtonText.setColor(sf::Color::White);
    saveButtonText.setPosition(470, 535);
    
    cancelButton.setSize(sf::Vector2f(200, 50));
    cancelButton.setPosition(650, 520);
    cancelButton.setFillColor(sf::Color(200, 50, 50));
    
    cancelButtonText.setFont(font);
    cancelButtonText.setString("Cancel");
    cancelButtonText.setCharacterSize(20);
    cancelButtonText.setColor(sf::Color::White);
    cancelButtonText.setPosition(720, 535);
}

void StoreApp::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void StoreApp::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
            
            switch (currentScreen) {
                case Screen::AUTH:
                    processAuthEvents(worldPos);
                    break;
                case Screen::REGISTER:
                    processRegisterEvents(worldPos);
                    break;
                case Screen::CUSTOMER:
                    processCustomerEvents(worldPos);
                    break;
                case Screen::ADMIN:
                    processAdminEvents(worldPos);
                    break;
                case Screen::CART:
                    processCartEvents(worldPos);
                    break;
                case Screen::RECEIPT:
                    processReceiptEvents(worldPos);
                    break;
                case Screen::FEEDBACK:
                    processFeedbackEvents(worldPos);
                    break;
                case Screen::PRODUCT_FORM:
                    processProductFormEvents(worldPos);
                    break;
            }
        }
        
        // Обработка ввода текста
        if (event.type == sf::Event::TextEntered) {
            if (currentScreen == Screen::AUTH) {
                if (typingUsername) {
                    if (event.text.unicode == '\b') { // Backspace
                        if (!usernameText.getString().isEmpty()) {
                            std::string current = usernameText.getString();
                            current.pop_back();
                            usernameText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        usernameText.setString(usernameText.getString() + static_cast<char>(event.text.unicode));
                    }
                } else if (typingPassword) {
                    if (event.text.unicode == '\b') {
                        if (!passwordText.getString().isEmpty()) {
                            std::string current = passwordText.getString();
                            current.pop_back();
                            passwordText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        passwordText.setString(passwordText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            else if (currentScreen == Screen::REGISTER) {
                if (typingRegUsername) {
                    if (event.text.unicode == '\b') {
                        if (!regUsernameText.getString().isEmpty()) {
                            std::string current = regUsernameText.getString();
                            current.pop_back();
                            regUsernameText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        regUsernameText.setString(regUsernameText.getString() + static_cast<char>(event.text.unicode));
                    }
                } else if (typingRegPassword) {
                    if (event.text.unicode == '\b') {
                        if (!regPasswordText.getString().isEmpty()) {
                            std::string current = regPasswordText.getString();
                            current.pop_back();
                            regPasswordText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        regPasswordText.setString(regPasswordText.getString() + static_cast<char>(event.text.unicode));
                    }
                } else if (typingRegConfirm) {
                    if (event.text.unicode == '\b') {
                        if (!regConfirmText.getString().isEmpty()) {
                            std::string current = regConfirmText.getString();
                            current.pop_back();
                            regConfirmText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        regConfirmText.setString(regConfirmText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            else if (currentScreen == Screen::PRODUCT_FORM) {
                if (typingName) {
                    if (event.text.unicode == '\b') {
                        if (!nameText.getString().isEmpty()) {
                            std::string current = nameText.getString();
                            current.pop_back();
                            nameText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        nameText.setString(nameText.getString() + static_cast<char>(event.text.unicode));
                    }
                } else if (typingPrice) {
                    if (event.text.unicode == '\b') {
                        if (!priceText.getString().isEmpty()) {
                            std::string current = priceText.getString();
                            current.pop_back();
                            priceText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        char c = static_cast<char>(event.text.unicode);
                        if (std::isdigit(c) || c == '.' || c == ',') {
                            priceText.setString(priceText.getString() + c);
                        }
                    }
                } else if (typingQuantity) {
                    if (event.text.unicode == '\b') {
                        if (!quantityText.getString().isEmpty()) {
                            std::string current = quantityText.getString();
                            current.pop_back();
                            quantityText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        char c = static_cast<char>(event.text.unicode);
                        if (std::isdigit(c)) {
                            quantityText.setString(quantityText.getString() + c);
                        }
                    }
                } else if (typingSize) {
                    if (event.text.unicode == '\b') {
                        if (!sizeText.getString().isEmpty()) {
                            std::string current = sizeText.getString();
                            current.pop_back();
                            sizeText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        sizeText.setString(sizeText.getString() + static_cast<char>(event.text.unicode));
                    }
                } else if (typingColor) {
                    if (event.text.unicode == '\b') {
                        if (!colorText.getString().isEmpty()) {
                            std::string current = colorText.getString();
                            current.pop_back();
                            colorText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        colorText.setString(colorText.getString() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            else if (currentScreen == Screen::FEEDBACK) {
                if (typingFeedback) {
                    if (event.text.unicode == '\b') {
                        if (!feedbackText.getString().isEmpty()) {
                            std::string current = feedbackText.getString();
                            current.pop_back();
                            feedbackText.setString(current);
                        }
                    } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        // Ограничиваем длину отзыва
                        if (feedbackText.getString().getSize() < 500) {
                            feedbackText.setString(feedbackText.getString() + static_cast<char>(event.text.unicode));
                        }
                    }
                }
            }
        }
        
        // Клик по полям ввода
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
            
            if (currentScreen == Screen::AUTH) {
                typingUsername = usernameBox.getGlobalBounds().contains(worldPos);
                typingPassword = passwordBox.getGlobalBounds().contains(worldPos);
                
                usernameBox.setOutlineColor(typingUsername ? sf::Color::Cyan : sf::Color::White);
                passwordBox.setOutlineColor(typingPassword ? sf::Color::Cyan : sf::Color::White);
            }
            else if (currentScreen == Screen::REGISTER) {
                typingRegUsername = regUsernameBox.getGlobalBounds().contains(worldPos);
                typingRegPassword = regPasswordBox.getGlobalBounds().contains(worldPos);
                typingRegConfirm = regConfirmBox.getGlobalBounds().contains(worldPos);
                
                regUsernameBox.setOutlineColor(typingRegUsername ? sf::Color::Cyan : sf::Color::White);
                regPasswordBox.setOutlineColor(typingRegPassword ? sf::Color::Cyan : sf::Color::White);
                regConfirmBox.setOutlineColor(typingRegConfirm ? sf::Color::Cyan : sf::Color::White);
            }
            else if (currentScreen == Screen::PRODUCT_FORM) {
                typingName = nameBox.getGlobalBounds().contains(worldPos);
                typingPrice = priceBox.getGlobalBounds().contains(worldPos);
                typingQuantity = quantityBox.getGlobalBounds().contains(worldPos);
                typingSize = sizeBox.getGlobalBounds().contains(worldPos);
                typingColor = colorBox.getGlobalBounds().contains(worldPos);
                typingCategory = categoryBox.getGlobalBounds().contains(worldPos);
                
                nameBox.setOutlineColor(typingName ? sf::Color::Cyan : sf::Color::White);
                priceBox.setOutlineColor(typingPrice ? sf::Color::Cyan : sf::Color::White);
                quantityBox.setOutlineColor(typingQuantity ? sf::Color::Cyan : sf::Color::White);
                sizeBox.setOutlineColor(typingSize ? sf::Color::Cyan : sf::Color::White);
                colorBox.setOutlineColor(typingColor ? sf::Color::Cyan : sf::Color::White);
                categoryBox.setOutlineColor(typingCategory ? sf::Color::Cyan : sf::Color::White);
            }
            else if (currentScreen == Screen::FEEDBACK) {
                typingFeedback = feedbackBox.getGlobalBounds().contains(worldPos);
                feedbackBox.setOutlineColor(typingFeedback ? sf::Color::Cyan : sf::Color::White);
            }
        }
    }
}

void StoreApp::processAuthEvents(const sf::Vector2f& mousePos) {
    // Клик по кнопке входа покупателя
    if (loginButton.getGlobalBounds().contains(mousePos)) {
        std::string username = usernameText.getString();
        std::string password = passwordText.getString();
        
        if (username.empty() || password.empty()) {
            showMessage("Please enter username and password");
            return;
        }
        
        currentUser = database.authenticateUser(username, password);
        if (currentUser && currentUser->isCustomer()) {
            clearInputFields();
            currentScreen = Screen::CUSTOMER;
            updateCustomerDisplay();
            showMessage("", sf::Color::Red);
        } else {
            showMessage("Invalid username or password");
        }
    }
    // Клик по кнопке регистрации
    else if (registerButton.getGlobalBounds().contains(mousePos)) {
        clearInputFields();
        currentScreen = Screen::REGISTER;
        showMessage("", sf::Color::Red);
    }
    // Клик по кнопке входа админа
    else if (adminButton.getGlobalBounds().contains(mousePos)) {
        std::string username = usernameText.getString();
        std::string password = passwordText.getString();
        
        if (username.empty() || password.empty()) {
            showMessage("Please enter username and password");
            return;
        }
        
        currentUser = database.authenticateUser(username, password);
        if (currentUser && currentUser->isSeller()) {
            clearInputFields();
            currentScreen = Screen::ADMIN;
            updateAdminDisplay();
            showMessage("", sf::Color::Red);
        } else {
            showMessage("Admin access denied");
        }
    }
}

void StoreApp::processRegisterEvents(const sf::Vector2f& mousePos) {
    // Клик по кнопке создания аккаунта
    if (createAccountButton.getGlobalBounds().contains(mousePos)) {
        std::string username = regUsernameText.getString();
        std::string password = regPasswordText.getString();
        std::string confirm = regConfirmText.getString();
        
        if (username.empty() || password.empty() || confirm.empty()) {
            showMessage("Please fill all fields");
            return;
        }
        
        if (password != confirm) {
            showMessage("Passwords do not match");
            return;
        }
        
        if (password.length() < 6) {
            showMessage("Password must be at least 6 characters");
            return;
        }
        
        if (database.registerUser(username, password, User::Role::CUSTOMER, 500.0f)) {
            showMessage("Account created successfully! You can now login.", sf::Color::Green);
            clearInputFields();
            currentScreen = Screen::AUTH;
        } else {
            showMessage("Username already exists");
        }
    }
    // Клик по кнопке назад
    else if (backButton.getGlobalBounds().contains(mousePos)) {
        clearInputFields();
        currentScreen = Screen::AUTH;
        showMessage("", sf::Color::Red);
    }
}

void StoreApp::processCustomerEvents(const sf::Vector2f& mousePos) {
    // Клик по товарам для добавления в корзину
    for (size_t i = 0; i < productBackgrounds.size(); ++i) {
        if (productBackgrounds[i].getGlobalBounds().contains(mousePos)) {
            selectedProductIndex = i;
            
            // Добавляем товар в корзину
            try {
                cart.addItem(products[i], 1);
                updateCustomerDisplay();
                showMessage("Added to cart: " + products[i]->getName(), sf::Color::Green);
            } catch (const std::exception& e) {
                showMessage(e.what());
            }
            break;
        }
    }
    
    // Клик по кнопке просмотра корзины
    if (viewCartButton.getGlobalBounds().contains(mousePos)) {
        previousScreen = Screen::CUSTOMER;
        currentScreen = Screen::CART;
        updateCartDisplay();
    }
    
    // Клик по кнопке выхода
    if (logoutButton.getGlobalBounds().contains(mousePos)) {
        currentUser.reset();
        cart.clear();
        currentScreen = Screen::AUTH;
        clearInputFields();
    }
}

void StoreApp::processAdminEvents(const sf::Vector2f& mousePos) {
    // Клик по товарам для выбора
    for (size_t i = 0; i < adminProductBackgrounds.size(); ++i) {
        if (adminProductBackgrounds[i].getGlobalBounds().contains(mousePos)) {
            selectedAdminProductIndex = i;
            updateAdminDisplay();
            break;
        }
    }
    
    // Кнопки админа
    if (addProductButton.getGlobalBounds().contains(mousePos)) {
        showProductForm(false);
    }
    
    if (editProductButton.getGlobalBounds().contains(mousePos)) {
        if (selectedAdminProductIndex >= 0) {
            int productId = products[selectedAdminProductIndex]->getId();
            showProductForm(true, productId);
        } else {
            showMessage("Select a product first");
        }
    }
    
    if (deleteProductButton.getGlobalBounds().contains(mousePos)) {
        if (selectedAdminProductIndex >= 0) {
            int productId = products[selectedAdminProductIndex]->getId();
            if (database.deleteProduct(productId)) {
                products = database.getAllProducts();
                selectedAdminProductIndex = -1;
                updateAdminDisplay();
                showMessage("Product deleted", sf::Color::Green);
            }
        } else {
            showMessage("Select a product first");
        }
    }
    
    if (adminLogoutButton.getGlobalBounds().contains(mousePos)) {
        currentUser.reset();
        currentScreen = Screen::AUTH;
        clearInputFields();
    }
}

void StoreApp::processCartEvents(const sf::Vector2f& mousePos) {
    // Клик по кнопке оформления заказа
    if (checkoutButton.getGlobalBounds().contains(mousePos)) {
        if (cart.isEmpty()) {
            showMessage("Cart is empty");
            return;
        }
        
        if (!currentUser) {
            showMessage("Not logged in");
            return;
        }
        
        // Создаем список товаров для передачи в createOrderDirect
        std::vector<std::pair<int, int>> orderItems;
        const auto& cartItems = cart.getItems();
        
        for (const auto& item : cartItems) {
            orderItems.push_back({item.product->getId(), item.quantity});
        }
        
        // Вызываем createOrderDirect для оформления заказа в БД
        bool purchaseSuccess = database.createOrderDirect(currentUser->getId(), orderItems);
        
        if (purchaseSuccess) {
            // Обновляем пользователя из базы данных
            auto updatedUser = database.getUserById(currentUser->getId());
            if (updatedUser) {
                currentUser->setBalance(updatedUser->getBalance());
            }
            
            // Обновляем список товаров
            products = database.getAllProducts();
            
            // Показываем чек
            currentScreen = Screen::RECEIPT;
            showReceiptScreen();
            
            // Показываем сообщение об успехе
            showMessage("Purchase successful!", sf::Color::Green);
        } else {
            showMessage("Purchase failed. Please try again.");
        }
    }
    
    // Клик по кнопке продолжения покупок
    if (continueShoppingButton.getGlobalBounds().contains(mousePos)) {
        currentScreen = previousScreen;
        if (currentScreen == Screen::CUSTOMER) {
            updateCustomerDisplay();
        }
    }
    
    // Клик по кнопке очистки корзины
    if (clearCartButton.getGlobalBounds().contains(mousePos)) {
        cart.clear();
        updateCartDisplay();
        showMessage("Cart cleared", sf::Color::Green);
    }
}

void StoreApp::processReceiptEvents(const sf::Vector2f& mousePos) {
    if (receiptOkButton.getGlobalBounds().contains(mousePos)) {
        // Переходим к странице отзывов
        currentScreen = Screen::FEEDBACK;
        setupFeedbackScreen();
    }
}

void StoreApp::processFeedbackEvents(const sf::Vector2f& mousePos) {
    // Клик по звездам рейтинга
    if (mousePos.x >= 500 && mousePos.x <= 800 && mousePos.y >= 110 && mousePos.y <= 150) {
        int starIndex = (mousePos.x - 500) / 60;
        if (starIndex >= 0 && starIndex < 5) {
            feedbackRating = starIndex + 1;
            updateRatingStars();
        }
    }
    
    // Кнопка отправки отзыва
    if (submitFeedbackButton.getGlobalBounds().contains(mousePos)) {
        submitFeedback();
    }
    
    // Кнопка пропуска отзыва
    if (skipFeedbackButton.getGlobalBounds().contains(mousePos)) {
        currentScreen = Screen::CUSTOMER;
        updateCustomerDisplay();
    }
}

void StoreApp::processProductFormEvents(const sf::Vector2f& mousePos) {
    // Проверка кликов по полям ввода
    typingName = nameBox.getGlobalBounds().contains(mousePos);
    typingPrice = priceBox.getGlobalBounds().contains(mousePos);
    typingQuantity = quantityBox.getGlobalBounds().contains(mousePos);
    typingSize = sizeBox.getGlobalBounds().contains(mousePos);
    typingColor = colorBox.getGlobalBounds().contains(mousePos);
    typingCategory = categoryBox.getGlobalBounds().contains(mousePos);
    
    // Подсветка активных полей
    nameBox.setOutlineColor(typingName ? sf::Color::Cyan : sf::Color::White);
    priceBox.setOutlineColor(typingPrice ? sf::Color::Cyan : sf::Color::White);
    quantityBox.setOutlineColor(typingQuantity ? sf::Color::Cyan : sf::Color::White);
    sizeBox.setOutlineColor(typingSize ? sf::Color::Cyan : sf::Color::White);
    colorBox.setOutlineColor(typingColor ? sf::Color::Cyan : sf::Color::White);
    categoryBox.setOutlineColor(typingCategory ? sf::Color::Cyan : sf::Color::White);
    
    // Клик по полю категории для переключения
    if (typingCategory) {
        selectedCategoryIndex = (selectedCategoryIndex + 1) % categoryNames.size();
        categoryText.setString(categoryNames[selectedCategoryIndex]);
    }
    
    // Кнопка сохранения
    if (saveButton.getGlobalBounds().contains(mousePos)) {
        saveProduct();
    }
    
    // Кнопка отмены
    if (cancelButton.getGlobalBounds().contains(mousePos)) {
        currentScreen = previousScreen;
        if (currentScreen == Screen::ADMIN) {
            updateAdminDisplay();
        }
    }
}

void StoreApp::update() {
    // Обновление логики (пока не нужно)
}

void StoreApp::render() {
    window.clear(sf::Color(20, 20, 40));
    
    switch (currentScreen) {
        case Screen::AUTH:
            renderAuthScreen();
            break;
        case Screen::REGISTER:
            renderRegisterScreen();
            break;
        case Screen::CUSTOMER:
            renderCustomerScreen();
            break;
        case Screen::ADMIN:
            renderAdminScreen();
            break;
        case Screen::CART:
            renderCartScreen();
            break;
        case Screen::RECEIPT:
            renderReceiptScreen();
            break;
        case Screen::FEEDBACK:
            renderFeedbackScreen();
            break;
        case Screen::PRODUCT_FORM:
            renderProductForm();
            break;
    }
    
    window.display();
}

void StoreApp::renderAuthScreen() {
    window.draw(authTitle);
    window.draw(usernameLabel);
    window.draw(passwordLabel);
    window.draw(usernameBox);
    window.draw(passwordBox);
    window.draw(usernameText);
    window.draw(passwordText);
    window.draw(loginButton);
    window.draw(loginButtonText);
    window.draw(registerButton);
    window.draw(registerButtonText);
    window.draw(adminButton);
    window.draw(adminButtonText);
    window.draw(messageText);
}

void StoreApp::renderRegisterScreen() {
    window.draw(registerTitle);
    window.draw(regUsernameLabel);
    window.draw(regPasswordLabel);
    window.draw(regConfirmLabel);
    window.draw(regUsernameBox);
    window.draw(regPasswordBox);
    window.draw(regConfirmBox);
    window.draw(regUsernameText);
    window.draw(regPasswordText);
    window.draw(regConfirmText);
    window.draw(createAccountButton);
    window.draw(createAccountText);
    window.draw(backButton);
    window.draw(backButtonText);
    window.draw(messageText);
}

void StoreApp::renderCustomerScreen() {
    window.draw(customerTitle);
    window.draw(balanceLabel);
    window.draw(productsBackground);
    
    for (const auto& bg : productBackgrounds) {
        window.draw(bg);
    }
    
    for (const auto& text : productDisplayTexts) {
        window.draw(text);
    }
    
    window.draw(viewCartButton);
    window.draw(viewCartText);
    window.draw(logoutButton);
    window.draw(logoutText);
    
    window.draw(messageText);
}

void StoreApp::renderAdminScreen() {
    window.draw(adminTitle);
    window.draw(adminBalanceLabel);
    window.draw(adminProductsBackground);
    
    for (const auto& bg : adminProductBackgrounds) {
        window.draw(bg);
    }
    
    for (const auto& text : adminProductTexts) {
        window.draw(text);
    }
    
    window.draw(addProductButton);
    window.draw(addProductText);
    window.draw(editProductButton);
    window.draw(editProductText);
    window.draw(deleteProductButton);
    window.draw(deleteProductText);
    window.draw(adminLogoutButton);
    window.draw(adminLogoutText);
    
    window.draw(messageText);
}

void StoreApp::renderCartScreen() {
    window.draw(cartTitle);
    window.draw(cartBalanceLabel);
    window.draw(cartTotalLabel);
    window.draw(cartItemsBackground);
    
    for (const auto& text : cartItemTexts) {
        window.draw(text);
    }
    
    window.draw(checkoutButton);
    window.draw(checkoutText);
    window.draw(continueShoppingButton);
    window.draw(continueShoppingText);
    window.draw(clearCartButton);
    window.draw(clearCartText);
    
    window.draw(messageText);
}

void StoreApp::renderReceiptScreen() {
    window.draw(receiptTitle);
    window.draw(receiptBackground);
    window.draw(receiptDetails);
    window.draw(receiptOkButton);
    window.draw(receiptOkText);
}

void StoreApp::renderFeedbackScreen() {
    window.draw(feedbackTitle);
    window.draw(ratingLabel);
    window.draw(ratingStars);
    window.draw(feedbackLabel);
    window.draw(feedbackBox);
    window.draw(feedbackText);
    window.draw(submitFeedbackButton);
    window.draw(submitFeedbackText);
    window.draw(skipFeedbackButton);
    window.draw(skipFeedbackText);
    
    window.draw(messageText);
}

void StoreApp::renderProductForm() {
    window.draw(productFormTitle);
    
    window.draw(nameLabel);
    window.draw(nameBox);
    window.draw(nameText);
    
    window.draw(categoryLabel);
    window.draw(categoryBox);
    window.draw(categoryText);
    
    window.draw(priceLabel);
    window.draw(priceBox);
    window.draw(priceText);
    
    window.draw(quantityLabel);
    window.draw(quantityBox);
    window.draw(quantityText);
    
    window.draw(sizeLabel);
    window.draw(sizeBox);
    window.draw(sizeText);
    
    window.draw(colorLabel);
    window.draw(colorBox);
    window.draw(colorText);
    
    window.draw(saveButton);
    window.draw(saveButtonText);
    window.draw(cancelButton);
    window.draw(cancelButtonText);
    
    window.draw(messageText);
}

void StoreApp::updateCustomerDisplay() {
    // Обновляем баланс
    if (currentUser) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Balance: $" << currentUser->getBalance();
        balanceLabel.setString(ss.str());
    }
    
    // Очищаем старые отображения
    productDisplayTexts.clear();
    productBackgrounds.clear();
    
    // Создаем отображение товаров
    float startY = 130.0f;
    for (size_t i = 0; i < products.size(); ++i) {
        // Фон товара
        sf::RectangleShape productBg(sf::Vector2f(1060, 50));
        productBg.setPosition(70, startY + i * 60);
        productBg.setFillColor(i == selectedProductIndex ? sf::Color(60, 60, 80) : sf::Color(50, 50, 70));
        productBg.setOutlineColor(sf::Color::White);
        productBg.setOutlineThickness(1);
        productBackgrounds.push_back(productBg);
        
        // Текст товара
        sf::Text productText;
        productText.setFont(font);
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << products[i]->getName()
           << " | Category: " << products[i]->getCategoryString()
           << " | Price: $" << products[i]->getPrice()
           << " | HP: " << products[i]->getSize()
           << " | Color: " << products[i]->getColor()
           << " | Available: " << products[i]->getQuantity();
        
        productText.setString(ss.str());
        productText.setCharacterSize(18);
        productText.setColor(sf::Color::White);
        productText.setPosition(80, startY + i * 60 + 10);
        productDisplayTexts.push_back(productText);
    }
}

void StoreApp::updateAdminDisplay() {
    // Обновляем баланс админа
    if (currentUser) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Store Balance: $" << currentUser->getBalance();
        adminBalanceLabel.setString(ss.str());
    }
    
    // Очищаем старые отображения
    adminProductTexts.clear();
    adminProductBackgrounds.clear();
    
    // Создаем отображение товаров для админа
    float startY = 130.0f;
    for (size_t i = 0; i < products.size(); ++i) {
        // Фон товара
        sf::RectangleShape productBg(sf::Vector2f(1060, 50));
        productBg.setPosition(70, startY + i * 60);
        productBg.setFillColor(i == selectedAdminProductIndex ? sf::Color(60, 60, 100) : sf::Color(50, 50, 70));
        productBg.setOutlineColor(sf::Color::White);
        productBg.setOutlineThickness(1);
        adminProductBackgrounds.push_back(productBg);
        
        // Текст товара
        sf::Text productText;
        productText.setFont(font);
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "ID: " << products[i]->getId()
           << " | " << products[i]->getName()
           << " | Type: " << products[i]->getCategoryString()
           << " | Price: $" << products[i]->getPrice()
           << " | Qty: " << products[i]->getQuantity()
           << " | HP: " << products[i]->getSize()
           << " | Color: " << products[i]->getColor();
        
        productText.setString(ss.str());
        productText.setCharacterSize(18);
        productText.setColor(sf::Color::White);
        productText.setPosition(80, startY + i * 60 + 10);
        adminProductTexts.push_back(productText);
    }
}

void StoreApp::updateCartDisplay() {
    // Обновляем баланс
    if (currentUser) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Balance: $" << currentUser->getBalance();
        cartBalanceLabel.setString(ss.str());
    }
    
    // Обновляем итоговую сумму
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Total: $" << cart.getTotalPrice();
    cartTotalLabel.setString(ss.str());
    
    // Очищаем старые отображения
    cartItemTexts.clear();
    
    // Создаем отображение товаров в корзине
    const auto& items = cart.getItems();
    float startY = 170.0f;
    
    for (size_t i = 0; i < items.size(); ++i) {
        sf::Text itemText;
        itemText.setFont(font);
        
        std::stringstream itemss;
        itemss << std::fixed << std::setprecision(2);
        itemss << items[i].product->getName()
               << " x" << items[i].quantity
               << " | $" << items[i].product->getPrice() << " each"
               << " | Total: $" << items[i].totalPrice;
        
        itemText.setString(itemss.str());
        itemText.setCharacterSize(20);
        itemText.setColor(sf::Color::White);
        itemText.setPosition(70, startY + i * 40);
        cartItemTexts.push_back(itemText);
    }
}

void StoreApp::showReceiptScreen() {
    // Получаем актуальные данные об админе
    auto admin = database.getUserByUsername("admin");
    float adminBalance = 0.0f;
    if (admin) {
        adminBalance = admin->getBalance();
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "Purchase Receipt\n";
    ss << "================\n";
    ss << "Date: " << __DATE__ << "\n";
    ss << "Time: " << __TIME__ << "\n";
    ss << "Customer: " << (currentUser ? currentUser->getUsername() : "Unknown") << "\n";
    ss << "================\n\n";
    
    const auto& items = cart.getItems();
    for (size_t i = 0; i < items.size(); ++i) {
        ss << items[i].product->getName()
           << " x" << items[i].quantity
           << " @ $" << items[i].product->getPrice()
           << " = $" << items[i].totalPrice << "\n";
    }
    
    ss << "\n================\n";
    ss << "Subtotal: $" << cart.getTotalPrice() << "\n";
    ss << "New Balance: $" << (currentUser ? currentUser->getBalance() : 0) << "\n";
    ss << "================\n";
    ss << "Money transferred to admin: $" << cart.getTotalPrice() << "\n";
    ss << "Admin balance: $" << adminBalance << "\n";
    ss << "================\n";
    ss << "Thank you for your purchase!";
    
    receiptDetails.setString(ss.str());
}

void StoreApp::setupFeedbackScreen() {
    // Сбросить поля отзыва
    feedbackRating = 5;
    feedbackText.setString("");
    typingFeedback = false;
    feedbackBox.setOutlineColor(sf::Color::White);
    updateRatingStars();
    showMessage("");
}

void StoreApp::updateRatingStars() {
    std::string stars = "";
    for (int i = 0; i < 5; i++) {
        if (i < feedbackRating) {
            stars += "*";
        } else {
            stars += "_";
        }
    }
    ratingStars.setString(stars);
}

void StoreApp::submitFeedback() {
    std::string feedback = feedbackText.getString();
    
    if (feedback.empty()) {
        showMessage("Please write your feedback");
        return;
    }
    
    // Здесь можно добавить сохранение отзыва в базу данных
    if (currentUser) {
        // Сохраняем отзыв в БД (нужно добавить соответствующий метод в Database)
        if (database.addFeedback(currentUser->getId(), feedback, feedbackRating)) {
            showMessage("Thank you for your feedback!", sf::Color::Green);
        } else {
            showMessage("Failed to save feedback");
            return;
        }
    }
    
    // Очищаем корзину после успешного отзыва
    cart.clear();
    
    // Возвращаемся к товарам
    currentScreen = Screen::CUSTOMER;
    updateCustomerDisplay();
}

void StoreApp::showProductForm(bool editing, int productId) {
    isEditingProduct = editing;
    editingProductId = productId;
    
    // Сбросить поля
    nameText.setString("");
    priceText.setString("");
    quantityText.setString("");
    sizeText.setString("");
    colorText.setString("");
    selectedCategoryIndex = 0;
    categoryText.setString(categoryNames[selectedCategoryIndex]);
    
    // Если редактирование, заполнить поля данными товара
    if (editing && productId != -1) {
        auto product = database.getProductById(productId);
        if (product) {
            nameText.setString(product->getName());
            
            std::stringstream priceStream;
            priceStream << std::fixed << std::setprecision(2) << product->getPrice();
            priceText.setString(priceStream.str());
            
            quantityText.setString(std::to_string(product->getQuantity()));
            sizeText.setString(product->getSize());
            colorText.setString(product->getColor());
            
            // Установить категорию
            int category = static_cast<int>(product->getCategory());
            if (category >= 0 && category < static_cast<int>(categoryNames.size())) {
                selectedCategoryIndex = category;
                categoryText.setString(categoryNames[selectedCategoryIndex]);
            }
            
            productFormTitle.setString("Edit Car Info");
        }
    } else {
        productFormTitle.setString("Add Car");
    }
    
    // Сбросить флаги ввода
    typingName = false;
    typingPrice = false;
    typingQuantity = false;
    typingSize = false;
    typingColor = false;
    typingCategory = false;
    
    // Сбросить подсветку полей
    nameBox.setOutlineColor(sf::Color::White);
    priceBox.setOutlineColor(sf::Color::White);
    quantityBox.setOutlineColor(sf::Color::White);
    sizeBox.setOutlineColor(sf::Color::White);
    colorBox.setOutlineColor(sf::Color::White);
    categoryBox.setOutlineColor(sf::Color::White);
    
    // Сохранить предыдущий экран и перейти к форме
    previousScreen = currentScreen;
    currentScreen = Screen::PRODUCT_FORM;
}

void StoreApp::saveProduct() {
    // Получаем данные из полей ввода
    std::string name = nameText.getString();
    std::string priceStr = priceText.getString();
    std::string quantityStr = quantityText.getString();
    std::string size = sizeText.getString();
    std::string color = colorText.getString();
    
    // Проверяем обязательные поля
    if (name.empty() || priceStr.empty() || quantityStr.empty()) {
        showMessage("Name, price and quantity are required");
        return;
    }
    
    // Парсим цену и количество
    float price;
    int quantity;
    
    try {
        // Заменяем запятую на точку для корректного парсинга
        std::replace(priceStr.begin(), priceStr.end(), ',', '.');
        price = std::stof(priceStr);
        quantity = std::stoi(quantityStr);
    } catch (const std::exception& e) {
        showMessage("Invalid price or quantity format");
        return;
    }
    
    // Проверяем валидность
    if (price <= 0) {
        showMessage("Price must be positive");
        return;
    }
    
    if (quantity < 0) {
        showMessage("Quantity cannot be negative");
        return;
    }
    
    // Создаем продукт
    Product::Category category = static_cast<Product::Category>(selectedCategoryIndex);
    
    bool success = false;
    if (isEditingProduct && editingProductId != -1) {
        // Редактирование существующего продукта
        Product updatedProduct(editingProductId, name, category, price, quantity, size, color);
        success = database.updateProduct(updatedProduct);
        if (success) {
            showMessage("Product updated successfully", sf::Color::Green);
        }
    } else {
        // Добавление нового продукта
        Product newProduct(0, name, category, price, quantity, size, color);
        success = database.addProduct(newProduct);
        if (success) {
            showMessage("Product added successfully", sf::Color::Green);
        }
    }
    
    if (success) {
        // Обновляем список продуктов
        products = database.getAllProducts();
        
        // Возвращаемся на предыдущий экран
        currentScreen = previousScreen;
        if (currentScreen == Screen::ADMIN) {
            updateAdminDisplay();
        }
    } else {
        showMessage("Failed to save product");
    }
}

void StoreApp::showMessage(const std::string& message, sf::Color color) {
    messageText.setString(message);
    messageText.setColor(color);
}

void StoreApp::clearInputFields() {
    usernameText.setString("");
    passwordText.setString("");
    regUsernameText.setString("");
    regPasswordText.setString("");
    regConfirmText.setString("");
    nameText.setString("");
    priceText.setString("");
    quantityText.setString("");
    sizeText.setString("");
    colorText.setString("");
    
    typingUsername = false;
    typingPassword = false;
    typingRegUsername = false;
    typingRegPassword = false;
    typingRegConfirm = false;
    typingName = false;
    typingPrice = false;
    typingQuantity = false;
    typingSize = false;
    typingColor = false;
    typingCategory = false;
    
    usernameBox.setOutlineColor(sf::Color::White);
    passwordBox.setOutlineColor(sf::Color::White);
    regUsernameBox.setOutlineColor(sf::Color::White);
    regPasswordBox.setOutlineColor(sf::Color::White);
    regConfirmBox.setOutlineColor(sf::Color::White);
    nameBox.setOutlineColor(sf::Color::White);
    priceBox.setOutlineColor(sf::Color::White);
    quantityBox.setOutlineColor(sf::Color::White);
    sizeBox.setOutlineColor(sf::Color::White);
    colorBox.setOutlineColor(sf::Color::White);
    categoryBox.setOutlineColor(sf::Color::White);
}

void StoreApp::updateCategoryDisplay() {
    // Метод оставлен для совместимости
}

// Методы show...Screen не используются в этой реализации,
// но оставляем их для совместимости с заголовочным файлом
void StoreApp::showAuthScreen() { currentScreen = Screen::AUTH; }
void StoreApp::showRegisterScreen() { currentScreen = Screen::REGISTER; }
void StoreApp::showCustomerScreen() { currentScreen = Screen::CUSTOMER; }
void StoreApp::showAdminScreen() { currentScreen = Screen::ADMIN; }
void StoreApp::showCartScreen() { currentScreen = Screen::CART; }
