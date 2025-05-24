#ifndef CART_HANDLER_H
#define CART_HANDLER_H

#include "menu_handler_base.h"
#include "shopping_cart.h"

class CartHandler : public MenuHandlerBase {
public:
    using MenuHandlerBase::MenuHandlerBase;
    
    void handleShoppingCart(const std::string& username);
    bool displayCart(const ShoppingCart& cart) const;
    void handleUpdateCartItem(ShoppingCart& cart);
    void handleRemoveCartItem(ShoppingCart& cart);
    void handleCheckout(ShoppingCart& cart, const std::string& username);
    void handleAddToCart(const std::shared_ptr<Product>& product, 
                        const std::string& username);
};

#endif