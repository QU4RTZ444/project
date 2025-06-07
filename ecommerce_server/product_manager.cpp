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
    if (type == "ʳƷ") {
        return std::make_unique<Food>(id, name, price, stock, merchant, discount);
    }
    else if (type == "�鼮") {
        return std::make_unique<Book>(id, name, price, stock, merchant, discount);
    }
    else if (type == "�·�") {
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
            std::cout << "��Ч����Ʒ����: " << type << std::endl;
            nextProductId--; // �ع�ID
            return false;
        }

        products.push_back(std::move(product));
        // �������浽�ļ�
        saveProductsToFile();

        std::cout << "��Ʒ��ӳɹ�: " << name << " (ID: " << (nextProductId - 1) << ", ����: " << type << ")";
        if (discount < 1.0) {
            std::cout << " [" << static_cast<int>(discount * 100) << "��]";
        }
        std::cout << std::endl;

        return true;
    }
    catch (const std::exception& e) {
        std::cout << "�����Ʒʧ��: " << e.what() << std::endl;
        nextProductId--; // �ع�ID
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

                // �������浽�ļ�
                saveProductsToFile();
                std::cout << "��Ʒ�޸ĳɹ�: " << product->getName() << " (ID: " << productId << ")" << std::endl;
                return true;
            }
            catch (const std::exception& e) {
                std::cout << "�޸���Ʒʧ��: " << e.what() << std::endl;
                return false;
            }
        }
    }

    std::cout << "��Ʒ������: ID " << productId << std::endl;
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
            // �������浽�ļ�
            saveProductsToFile();
            std::cout << "�ɹ�Ϊ " << modifiedCount << " ��" << productType
                << "��Ʒ���� " << static_cast<int>(discount * 100) << "��" << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cout << "�����ۿ�ʧ��: " << e.what() << std::endl;
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
        std::cout << "��Ʒ�ļ������ڣ�������ʾ����Ʒ: " << filename << std::endl;

        // ���ʾ����Ʒ - ÿ��3����Ʒ
        // ʳƷ��
        addProduct("ʳƷ", "ƻ��", 8.50, 100, "B", 1.0);
        addProduct("ʳƷ", "ţ��", 15.80, 50, "B", 0.9); // 9��
        addProduct("ʳƷ", "���", 12.00, 80, "B", 1.0);

        // �鼮��
        addProduct("�鼮", "C++ Primer", 89.00, 30, "B", 0.85); // 85��
        addProduct("�鼮", "Effective C++", 68.00, 25, "B", 1.0);
        addProduct("�鼮", "�����������ϵͳ", 139.00, 15, "B", 0.9); // 9��

        // �·���
        addProduct("�·�", "T��", 89.00, 200, "B", 0.7); // 7��
        addProduct("�·�", "ţ�п�", 299.00, 150, "B", 1.0);
        addProduct("�·�", "�˶�Ь", 499.00, 100, "B", 0.8); // 8��

        return;
    }

    products.clear();

    try {
        // ����ļ���С
        file.seekg(0, std::ios::end);
        std::streamsize fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize < sizeof(int) + sizeof(size_t)) {
            std::cerr << "��Ʒ�ļ���ʽ������ļ��𻵣������´���" << std::endl;
            file.close();
            // ɾ���𻵵��ļ�
            std::remove(filename.c_str());
            loadProducts(); // �ݹ�������´���
            return;
        }

        // ��ȡ��һ����ƷID
        file.read(reinterpret_cast<char*>(&nextProductId), sizeof(nextProductId));
        if (file.fail()) {
            throw std::runtime_error("��ȡ��ƷIDʧ��");
        }

        // ��ȡ��Ʒ����
        size_t productCount;
        file.read(reinterpret_cast<char*>(&productCount), sizeof(productCount));
        if (file.fail() || productCount > 10000) { // ��Ӻ����Լ��
            throw std::runtime_error("��ȡ��Ʒ����ʧ�ܻ������쳣");
        }

        std::cout << "׼������ " << productCount << " ����Ʒ..." << std::endl;

        for (size_t i = 0; i < productCount; ++i) {
            try {
                // �ȶ�ȡ��Ʒ����
                uint32_t typeLen;
                file.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
                if (file.fail() || typeLen > 1000) {
                    std::cerr << "��ȡ�� " << (i + 1) << " ����Ʒ���ͳ���ʧ�ܣ�����" << std::endl;
                    break;
                }

                std::string type;
                if (typeLen > 0) {
                    type.resize(typeLen);
                    file.read(&type[0], typeLen);
                    if (file.fail()) {
                        std::cerr << "��ȡ�� " << (i + 1) << " ����Ʒ����ʧ�ܣ�����" << std::endl;
                        break;
                    }
                }

                // ������Ӧ���͵���Ʒ����
                auto product = createProduct(type, 0, "", 0.0, 0, "");
                if (product) {
                    // ���¶�λ�����ͳ���λ�ÿ�ʼ�����л�
                    file.seekg(-(static_cast<std::streamoff>(sizeof(uint32_t) + typeLen)), std::ios::cur);
                    product->deserialize(file);
                    products.push_back(std::move(product));
                    std::cout << "�ɹ�������Ʒ " << (i + 1) << "/" << productCount << std::endl;
                }
                else {
                    std::cerr << "�޷�������Ʒ����: " << type << "�������� " << (i + 1) << " ����Ʒ" << std::endl;
                    break;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "���ص� " << (i + 1) << " ����Ʒʱ����: " << e.what() << "������" << std::endl;
                break;
            }
        }

        std::cout << "�ɹ����� " << products.size() << " ����Ʒ" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "������Ʒ�ļ�ʱ����: " << e.what() << std::endl;
        std::cerr << "��ɾ���𻵵��ļ������´���" << std::endl;
        products.clear();
        nextProductId = 1;
        file.close();

        // ɾ���𻵵��ļ�
        std::remove(filename.c_str());

        // ���´���ʾ����Ʒ
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
    // �ȱ��浽��ʱ�ļ�
    std::string tempFilename = filename + ".tmp";
    std::ofstream file(tempFilename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "�޷�����ʱ�ļ�����д��: " << tempFilename << std::endl;
        return;
    }

    try {
        // д����һ����ƷID
        file.write(reinterpret_cast<const char*>(&nextProductId), sizeof(nextProductId));

        // д����Ʒ����
        size_t productCount = products.size();
        file.write(reinterpret_cast<const char*>(&productCount), sizeof(productCount));

        // д��������Ʒ
        for (size_t i = 0; i < products.size(); ++i) {
            try {
                products[i]->serialize(file);
                if (file.fail()) {
                    throw std::runtime_error("д��� " + std::to_string(i + 1) + " ����Ʒʧ��");
                }
            }
            catch (const std::exception& e) {
                std::cerr << "���л��� " << (i + 1) << " ����Ʒʱ����: " << e.what() << std::endl;
                throw;
            }
        }

        file.close();

        // ����ɹ�д����ʱ�ļ������滻ԭ�ļ�
        std::remove(filename.c_str());
        if (std::rename(tempFilename.c_str(), filename.c_str()) != 0) {
            std::cerr << "�޷��滻��Ʒ�ļ�" << std::endl;
        }
        else {
            std::cout << "�ɹ����� " << products.size() << " ����Ʒ���ļ�" << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "������Ʒ�ļ�ʱ����: " << e.what() << std::endl;
        file.close();
        std::remove(tempFilename.c_str()); // ɾ����ʱ�ļ�
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

    // ��ɸѡ�����̼ҵ���Ʒ
    std::vector<Product*> merchantProducts;
    for (const auto& product : products) {
        if (product->getMerchantName() == merchantName) {
            merchantProducts.push_back(product.get());
        }
    }

    // ��ҳ
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