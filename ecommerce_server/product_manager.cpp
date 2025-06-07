#include "product_manager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

ProductManager::ProductManager(const std::string& filename)
    : filename(filename), nextProductId(1) {
    loadProducts();
}

ProductManager::~ProductManager() {
    saveProducts();
}

std::unique_ptr<Product> ProductManager::createProduct(const std::string& type, int id,
    const std::string& name, double price,
    int stock, const std::string& merchant,
    double discount) {
    if (type == "食品") {
        return std::make_unique<Food>(id, name, price, stock, merchant, discount);
    }
    else if (type == "书籍") {
        return std::make_unique<Book>(id, name, price, stock, merchant, discount);
    }
    else if (type == "衣服") {
        return std::make_unique<Clothing>(id, name, price, stock, merchant, discount);
    }
    return nullptr;
}

bool ProductManager::addProduct(const std::string& type, const std::string& name,
    double price, int stock, const std::string& merchantName,
    double discount) {
    std::lock_guard<std::mutex> lock(productsMutex);

    try {
        auto product = createProduct(type, nextProductId++, name, price, stock, merchantName, discount);
        if (!product) {
            std::cout << "无效的商品类型: " << type << std::endl;
            nextProductId--; // 回滚ID
            return false;
        }

        products.push_back(std::move(product));
        // 立即保存到文件
        saveProductsToFile();

        std::cout << "商品添加成功: " << name << " (ID: " << (nextProductId - 1) << ", 类型: " << type << ")";
        if (discount < 1.0) {
            std::cout << " [" << static_cast<int>(discount * 100) << "折]";
        }
        std::cout << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "添加商品失败: " << e.what() << std::endl;
        nextProductId--; // 回滚ID
        return false;
    }
}

bool ProductManager::modifyProduct(int productId, double newPrice, int newStock, double newDiscount) {
    std::lock_guard<std::mutex> lock(productsMutex);

    for (auto& product : products) {
        if (product->getProductId() == productId) {
            try {
                if (newPrice >= 0) {
                    product->setPrice(newPrice);
                }
                if (newStock >= 0) {
                    product->setStock(newStock);
                }
                if (newDiscount >= 0) {
                    product->setDiscount(newDiscount);
                }

                // 立即保存到文件
                saveProductsToFile();
                std::cout << "商品修改成功: " << product->getName() << " (ID: " << productId << ")" << std::endl;
                return true;
            }
            catch (const std::exception& e) {
                std::cout << "修改商品失败: " << e.what() << std::endl;
                return false;
            }
        }
    }

    std::cout << "商品不存在: ID " << productId << std::endl;
    return false;
}

int ProductManager::setDiscountByType(const std::string& productType, double discount) {
    std::lock_guard<std::mutex> lock(productsMutex);

    int modifiedCount = 0;

    try {
        for (auto& product : products) {
            if (product->getProductType() == productType) {
                product->setDiscount(discount);
                modifiedCount++;
            }
        }

        if (modifiedCount > 0) {
            // 立即保存到文件
            saveProductsToFile();
            std::cout << "成功为 " << modifiedCount << " 个" << productType
                << "商品设置 " << static_cast<int>(discount * 100) << "折" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "设置折扣失败: " << e.what() << std::endl;
        return 0;
    }

    return modifiedCount;
}

std::vector<ProductInfo> ProductManager::getAllProducts() const {
    std::lock_guard<std::mutex> lock(productsMutex);

    std::vector<ProductInfo> result;
    for (const auto& product : products) {
        result.emplace_back(*product);
    }
    return result;
}

std::vector<ProductInfo> ProductManager::getProductsByPage(int page, int pageSize) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    std::vector<ProductInfo> result;
    int startIndex = (page - 1) * pageSize;
    int endIndex = std::min(startIndex + pageSize, static_cast<int>(products.size()));

    for (int i = startIndex; i < endIndex; ++i) {
        result.emplace_back(*products[i]);
    }

    return result;
}

std::vector<ProductInfo> ProductManager::searchProducts(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    std::vector<ProductInfo> result;
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(), lowerKeyword.begin(), ::tolower);

    for (const auto& product : products) {
        std::string productName = product->getName();
        std::string productType = product->getProductType();

        std::transform(productName.begin(), productName.end(), productName.begin(), ::tolower);
        std::transform(productType.begin(), productType.end(), productType.begin(), ::tolower);

        if (productName.find(lowerKeyword) != std::string::npos ||
            productType.find(lowerKeyword) != std::string::npos) {
            result.emplace_back(*product);
        }
    }

    return result;
}

std::vector<ProductInfo> ProductManager::getProductsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    std::vector<ProductInfo> result;
    for (const auto& product : products) {
        if (product->getProductType() == type) {
            result.emplace_back(*product);
        }
    }

    return result;
}

Product* ProductManager::getProductById(int productId) {
    std::lock_guard<std::mutex> lock(productsMutex);

    for (auto& product : products) {
        if (product->getProductId() == productId) {
            return product.get();
        }
    }
    return nullptr;
}

size_t ProductManager::getProductCount() const {
    std::lock_guard<std::mutex> lock(productsMutex);
    return products.size();
}

int ProductManager::getTotalPages(int pageSize) const {
    size_t count = getProductCount();
    return static_cast<int>((count + pageSize - 1) / pageSize);
}

void ProductManager::loadProducts() {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "商品文件不存在，将创建示例商品: " << filename << std::endl;

        // 添加示例商品 - 每类3个商品
        // 食品类
        addProduct("食品", "苹果", 8.50, 100, "B", 1.0);
        addProduct("食品", "牛奶", 15.80, 50, "B", 0.9); // 9折
        addProduct("食品", "面包", 12.00, 80, "B", 1.0);

        // 书籍类
        addProduct("书籍", "C++ Primer", 89.00, 30, "B", 0.85); // 85折
        addProduct("书籍", "Effective C++", 68.00, 25, "B", 1.0);
        addProduct("书籍", "深入理解计算机系统", 139.00, 15, "B", 0.9); // 9折

        // 衣服类
        addProduct("衣服", "T恤", 89.00, 200, "B", 0.7); // 7折
        addProduct("衣服", "牛仔裤", 299.00, 150, "B", 1.0);
        addProduct("衣服", "运动鞋", 499.00, 100, "B", 0.8); // 8折

        return;
    }

    products.clear();

    try {
        // 检查文件大小
        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize < sizeof(int) + sizeof(size_t)) {
            std::cerr << "商品文件格式错误或文件损坏，将重新创建" << std::endl;
            file.close();
            // 删除损坏的文件
            std::remove(filename.c_str());
            loadProducts(); // 递归调用重新创建
            return;
        }

        // 读取下一个商品ID
        file.read(reinterpret_cast<char*>(&nextProductId), sizeof(nextProductId));
        if (file.fail()) {
            throw std::runtime_error("读取商品ID失败");
        }

        // 读取商品数量
        size_t productCount;
        file.read(reinterpret_cast<char*>(&productCount), sizeof(productCount));
        if (file.fail() || productCount > 10000) { // 添加合理性检查
            throw std::runtime_error("读取商品数量失败或数量异常");
        }

        std::cout << "准备加载 " << productCount << " 个商品..." << std::endl;

        for (size_t i = 0; i < productCount; ++i) {
            try {
                // 先读取商品类型
                uint32_t typeLen;
                file.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
                if (file.fail() || typeLen > 1000) {
                    std::cerr << "读取第 " << (i + 1) << " 个商品类型长度失败，跳过" << std::endl;
                    break;
                }

                std::string type;
                if (typeLen > 0) {
                    type.resize(typeLen);
                    file.read(&type[0], typeLen);
                    if (file.fail()) {
                        std::cerr << "读取第 " << (i + 1) << " 个商品类型失败，跳过" << std::endl;
                        break;
                    }
                }

                // 创建对应类型的商品对象
                auto product = createProduct(type, 0, "", 0.0, 0, "");
                if (product) {
                    // 重新定位到类型长度位置开始反序列化
                    file.seekg(-(static_cast<std::streamoff>(sizeof(uint32_t) + typeLen)), std::ios::cur);
                    product->deserialize(file);
                    products.push_back(std::move(product));
                    std::cout << "成功加载商品 " << (i + 1) << "/" << productCount << std::endl;
                }
                else {
                    std::cerr << "无法创建商品类型: " << type << "，跳过第 " << (i + 1) << " 个商品" << std::endl;
                    break;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "加载第 " << (i + 1) << " 个商品时出错: " << e.what() << "，跳过" << std::endl;
                break;
            }
        }

        std::cout << "成功加载 " << products.size() << " 个商品" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "加载商品文件时出错: " << e.what() << std::endl;
        std::cerr << "将删除损坏的文件并重新创建" << std::endl;
        products.clear();
        nextProductId = 1;
        file.close();

        // 删除损坏的文件
        std::remove(filename.c_str());

        // 重新创建示例商品
        loadProducts();
        return;
    }

    file.close();
}

void ProductManager::saveProducts() {
    std::lock_guard<std::mutex> lock(productsMutex);
    saveProductsToFile();
}

void ProductManager::saveProductsToFile() {
    // 先保存到临时文件
    std::string tempFilename = filename + ".tmp";
    std::ofstream file(tempFilename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开临时文件进行写入: " << tempFilename << std::endl;
        return;
    }

    try {
        // 写入下一个商品ID
        file.write(reinterpret_cast<const char*>(&nextProductId), sizeof(nextProductId));

        // 写入商品数量
        size_t productCount = products.size();
        file.write(reinterpret_cast<const char*>(&productCount), sizeof(productCount));

        // 写入所有商品
        for (size_t i = 0; i < products.size(); ++i) {
            try {
                products[i]->serialize(file);
                if (file.fail()) {
                    throw std::runtime_error("写入第 " + std::to_string(i + 1) + " 个商品失败");
                }
            }
            catch (const std::exception& e) {
                std::cerr << "序列化第 " << (i + 1) << " 个商品时出错: " << e.what() << std::endl;
                throw;
            }
        }

        file.close();

        // 如果成功写入临时文件，则替换原文件
        std::remove(filename.c_str());
        if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
            std::cerr << "无法替换商品文件" << std::endl;
        }
        else {
            std::cout << "成功保存 " << products.size() << " 个商品到文件" << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "保存商品文件时出错: " << e.what() << std::endl;
        file.close();
        std::remove(tempFilename.c_str()); // 删除临时文件
    }
}

std::vector<ProductInfo> ProductManager::getProductsByMerchant(const std::string& merchantName) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    std::vector<ProductInfo> result;
    for (const auto& product : products) {
        if (product->getMerchantName() == merchantName) {
            result.emplace_back(*product);
        }
    }

    return result;
}

std::vector<ProductInfo> ProductManager::getMerchantProductsByPage(const std::string& merchantName, int page, int pageSize) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    // 先筛选出该商家的商品
    std::vector<Product*> merchantProducts;
    for (const auto& product : products) {
        if (product->getMerchantName() == merchantName) {
            merchantProducts.push_back(product.get());
        }
    }

    // 分页
    std::vector<ProductInfo> result;
    int startIndex = (page - 1) * pageSize;
    int endIndex = std::min(startIndex + pageSize, static_cast<int>(merchantProducts.size()));

    for (int i = startIndex; i < endIndex; ++i) {
        result.emplace_back(*merchantProducts[i]);
    }

    return result;
}

int ProductManager::getMerchantProductCount(const std::string& merchantName) const {
    std::lock_guard<std::mutex> lock(productsMutex);

    int count = 0;
    for (const auto& product : products) {
        if (product->getMerchantName() == merchantName) {
            count++;
        }
    }

    return count;
}