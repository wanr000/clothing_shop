#include "product.hpp"
#include <stdexcept>
#include <iomanip>
#include <sstream>

Product::Product(int id, const std::string& name, Category category,
                 float price, int quantity, const std::string& size,
                 const std::string& color)
    : id(id), name(name), category(category), price(price),
      quantity(quantity), size(size), color(color) {
    if (price < 0) throw std::invalid_argument("Price cannot be negative");
    if (quantity < 0) throw std::invalid_argument("Quantity cannot be negative");
}

int Product::getId() const { return id; }
std::string Product::getName() const { return name; }
Product::Category Product::getCategory() const { return category; }
float Product::getPrice() const { return price; }
int Product::getQuantity() const { return quantity; }
std::string Product::getSize() const { return size; }
std::string Product::getColor() const { return color; }

void Product::setId(int id) { this->id = id; }
void Product::setName(const std::string& name) { this->name = name; }
void Product::setCategory(Category category) { this->category = category; }
void Product::setPrice(float price) {
    if (price < 0) throw std::invalid_argument("Price cannot be negative");
    this->price = price;
}
void Product::setQuantity(int quantity) {
    if (quantity < 0) throw std::invalid_argument("Quantity cannot be negative");
    this->quantity = quantity;
}
void Product::setSize(const std::string& size) { this->size = size; }
void Product::setColor(const std::string& color) { this->color = color; }

bool Product::isAvailable() const {
    return quantity > 0;
}

void Product::decreaseQuantity(int amount) {
    if (amount < 0) throw std::invalid_argument("Amount cannot be negative");
    if (amount > quantity) throw std::invalid_argument("Cannot decrease more than available quantity");
    quantity -= amount;
}

std::string Product::getCategoryString() const {
    switch (category) {
        case Category::SUPERCAR: return "Supercar";
        case Category::PANTS: return "Hypercar";
        case Category::DRESS: return "Family car";
        case Category::JACKET: return "Race car";
        case Category::SHOES: return "Collector's car";
        case Category::ACCESSORIES: return "just car";
        default: return "Unknown";
    }
}

