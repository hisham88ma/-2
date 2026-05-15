#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <string>

class InputError : public std::exception {
public:
    const char* what() const noexcept override {
        return "Invalid input.";
    }
};

class NotFoundError : public std::exception {
public:
    const char* what() const noexcept override {
        return "Requested item was not found.";
    }
};

class Shape {
public:
    virtual double area() const = 0;
    virtual const char* label() const = 0;
    virtual ~Shape() = default;
};

class Rectangle : public Shape {
    double width;
    double height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override { return width * height; }
    const char* label() const override { return "Rectangle"; }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() const override { return 3.14159 * radius * radius; }
    const char* label() const override { return "Circle"; }
};

class BookRecord {
    int id;
    std::string title;
    std::string author;
    int pages;
public:
    BookRecord(int id_, const std::string& t, const std::string& a, int p)
        : id(id_), title(t), author(a), pages(p) {}

    int getId() const { return id; }
    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    int getPages() const { return pages; }

    bool operator==(const BookRecord& other) const {
        return id == other.id;
    }

    friend std::ostream& operator<<(std::ostream& os, const BookRecord& book) {
        os << "[" << book.id << "] " << book.title
           << " by " << book.author << " (" << book.pages << " pages)";
        return os;
    }
};

template<typename T>
class SimpleStack {
    std::vector<T> values;
public:
    void push(const T& value) {
        values.push_back(value);
    }

    void pop() {
        if (values.empty()) throw InputError();
        values.pop_back();
    }

    const T& top() const {
        if (values.empty()) throw InputError();
        return values.back();
    }

    bool empty() const {
        return values.empty();
    }
};

class Archive {
    std::map<int, std::shared_ptr<BookRecord>> books;
public:
    void addBook(std::shared_ptr<BookRecord> book) {
        books[book->getId()] = std::move(book);
    }

    std::shared_ptr<BookRecord> findBook(int id) const {
        auto it = books.find(id);
        if (it == books.end()) throw NotFoundError();
        return it->second;
    }

    void removeBook(int id) {
        if (books.erase(id) == 0) throw NotFoundError();
    }

    void printAll() const {
        for (const auto& [key, book] : books) {
            std::cout << *book << '\n';
        }
    }

    std::vector<std::shared_ptr<BookRecord>> byAuthor(const std::string& authorName) const {
        std::vector<std::shared_ptr<BookRecord>> result;
        for (const auto& [key, book] : books) {
            if (book->getAuthor() == authorName) {
                result.push_back(book);
            }
        }
        return result;
    }

    void printSortedByPages() const {
        std::vector<std::shared_ptr<BookRecord>> sorted;
        for (const auto& [key, book] : books) {
            sorted.push_back(book);
        }

        std::sort(sorted.begin(), sorted.end(),
            [](const std::shared_ptr<BookRecord>& a,
               const std::shared_ptr<BookRecord>& b) {
                return a->getPages() < b->getPages();
            });

        std::cout << "\nBooks sorted by page count:\n";
        for (const auto& book : sorted) {
            std::cout << *book << '\n';
        }
    }
};

int main() {
    try {
        Archive archive;

        archive.addBook(std::make_shared<BookRecord>(101, "Modern C++", "John Smith", 320));
        archive.addBook(std::make_shared<BookRecord>(102, "Data Structures", "Jane Doe", 420));
        archive.addBook(std::make_shared<BookRecord>(103, "Software Design", "John Smith", 280));

        std::cout << "Library inventory:\n";
        archive.printAll();

        std::cout << "\nSearch results for author John Smith:\n";
        auto found = archive.byAuthor("John Smith");
        for (const auto& book : found) {
            std::cout << *book << '\n';
        }

        archive.printSortedByPages();

        SimpleStack<int> stack;
        stack.push(5);
        stack.push(15);
        std::cout << "\nStack top value: " << stack.top() << '\n';
        stack.pop();

        std::unique_ptr<Shape> box = std::make_unique<Rectangle>(4.0, 6.0);
        std::unique_ptr<Shape> disk = std::make_unique<Circle>(5.0);

        std::cout << "\n" << box->label() << " area: " << box->area() << '\n';
        std::cout << disk->label() << " area: " << disk->area() << '\n';

    } catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << '\n';
    }

    return 0;
}
