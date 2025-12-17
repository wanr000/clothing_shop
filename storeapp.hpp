#ifndef STOREAPP_HPP
#define STOREAPP_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "database.hpp"
#include "cart.hpp"

class StoreApp {
public:
    enum class Screen {
        AUTH,
        REGISTER,
        CUSTOMER,
        ADMIN,
        CART,
        RECEIPT,
        FEEDBACK,  // Добавлен экран отзывов
        PRODUCT_FORM
    };
    
    StoreApp();
    void run();
    
private:
    void setupUI();
    void processEvents();
    void update();
    void render();
    
    // Обработчики событий для каждого экрана
    void processAuthEvents(const sf::Vector2f& mousePos);
    void processRegisterEvents(const sf::Vector2f& mousePos);
    void processCustomerEvents(const sf::Vector2f& mousePos);
    void processAdminEvents(const sf::Vector2f& mousePos);
    void processCartEvents(const sf::Vector2f& mousePos);
    void processReceiptEvents(const sf::Vector2f& mousePos);
    void processFeedbackEvents(const sf::Vector2f& mousePos);  // Добавлен обработчик отзывов
    void processProductFormEvents(const sf::Vector2f& mousePos);
    
    // Методы отрисовки каждого экрана
    void renderAuthScreen();
    void renderRegisterScreen();
    void renderCustomerScreen();
    void renderAdminScreen();
    void renderCartScreen();
    void renderReceiptScreen();
    void renderFeedbackScreen();  // Добавлен метод отрисовки отзывов
    void renderProductForm();
    
    // Вспомогательные методы
    void updateCustomerDisplay();
    void updateAdminDisplay();
    void updateCartDisplay();
    void showReceiptScreen();
    void setupFeedbackScreen();  // Добавлен метод настройки экрана отзывов
    void updateRatingStars();    // Добавлен метод обновления звезд рейтинга
    void submitFeedback();       // Добавлен метод отправки отзыва
    
    void showProductForm(bool editing, int productId = -1);
    void saveProduct();
    void showMessage(const std::string& message, sf::Color color = sf::Color::Red);
    void clearInputFields();
    void updateCategoryDisplay();
    
    // Методы для совместимости с заголовочным файлом
    void showAuthScreen();
    void showRegisterScreen();
    void showCustomerScreen();
    void showAdminScreen();
    void showCartScreen();
    
private:
    sf::RenderWindow window;
    Database database;
    Cart cart;
    std::shared_ptr<User> currentUser;
    std::vector<std::shared_ptr<Product>> products;
    std::vector<std::string> categoryNames;
    
    Screen currentScreen;
    Screen previousScreen;
    Screen productEditScreen;
    
    // Выбранные элементы
    int selectedProductIndex;
    int selectedAdminProductIndex;
    
    // Состояние формы продукта
    bool isEditingProduct;
    int editingProductId;
    
    // Флаги ввода текста
    bool typingUsername;
    bool typingPassword;
    bool typingRegUsername;
    bool typingRegPassword;
    bool typingRegConfirm;
    bool typingName;
    bool typingPrice;
    bool typingQuantity;
    bool typingSize;
    bool typingColor;
    bool typingCategory;
    bool typingFeedback;  // Добавлен флаг ввода отзыва
    
    // Рейтинг для отзыва
    int feedbackRating;
    
    int selectedCategoryIndex;
    
    // Шрифт
    sf::Font font;
    
    // Элементы интерфейса для аутентификации
    sf::Text authTitle;
    sf::Text usernameLabel, passwordLabel;
    sf::RectangleShape usernameBox, passwordBox;
    sf::Text usernameText, passwordText;
    sf::RectangleShape loginButton, registerButton, adminButton;
    sf::Text loginButtonText, registerButtonText, adminButtonText;
    sf::Text messageText;
    
    // Элементы интерфейса для регистрации
    sf::Text registerTitle;
    sf::Text regUsernameLabel, regPasswordLabel, regConfirmLabel;
    sf::RectangleShape regUsernameBox, regPasswordBox, regConfirmBox;
    sf::Text regUsernameText, regPasswordText, regConfirmText;
    sf::RectangleShape createAccountButton, backButton;
    sf::Text createAccountText, backButtonText;
    
    // Элементы интерфейса для покупателя
    sf::Text customerTitle;
    sf::Text balanceLabel;
    sf::RectangleShape productsBackground;
    sf::RectangleShape viewCartButton, logoutButton;
    sf::Text viewCartText, logoutText;
    std::vector<sf::RectangleShape> productBackgrounds;
    std::vector<sf::Text> productDisplayTexts;
    
    // Элементы интерфейса для админа
    sf::Text adminTitle;
    sf::Text adminBalanceLabel;
    sf::RectangleShape adminProductsBackground;
    sf::RectangleShape addProductButton, editProductButton, deleteProductButton, adminLogoutButton;
    sf::Text addProductText, editProductText, deleteProductText, adminLogoutText;
    std::vector<sf::RectangleShape> adminProductBackgrounds;
    std::vector<sf::Text> adminProductTexts;
    
    // Элементы интерфейса для корзины
    sf::Text cartTitle;
    sf::Text cartBalanceLabel, cartTotalLabel;
    sf::RectangleShape cartItemsBackground;
    sf::RectangleShape checkoutButton, continueShoppingButton, clearCartButton;
    sf::Text checkoutText, continueShoppingText, clearCartText;
    std::vector<sf::Text> cartItemTexts;
    
    // Элементы интерфейса для чека
    sf::Text receiptTitle;
    sf::RectangleShape receiptBackground;
    sf::Text receiptDetails;
    sf::RectangleShape receiptOkButton;
    sf::Text receiptOkText;
    
    // Элементы интерфейса для отзывов (добавлены)
    sf::Text feedbackTitle;
    sf::Text ratingLabel;
    sf::Text ratingStars;
    sf::Text feedbackLabel;
    sf::RectangleShape feedbackBox;
    sf::Text feedbackText;
    sf::RectangleShape submitFeedbackButton, skipFeedbackButton;
    sf::Text submitFeedbackText, skipFeedbackText;
    
    // Элементы интерфейса для формы продукта
    sf::Text productFormTitle;
    sf::Text nameLabel, categoryLabel, priceLabel, quantityLabel, sizeLabel, colorLabel;
    sf::RectangleShape nameBox, categoryBox, priceBox, quantityBox, sizeBox, colorBox;
    sf::Text nameText, categoryText, priceText, quantityText, sizeText, colorText;
    sf::RectangleShape saveButton, cancelButton;
    sf::Text saveButtonText, cancelButtonText;
};

#endif // STOREAPP_HPP
