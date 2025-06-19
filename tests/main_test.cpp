#include <gtest/gtest.h>
#include <QApplication>

// Initialize Qt application for tests that might need it
int argc = 0;
char** argv = nullptr;
QApplication* app = nullptr;

// Global setup for all tests
class GlobalTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        // Initialize Qt Application for tests that need it
        if (!QApplication::instance()) {
            app = new QApplication(argc, argv);
        }
    }
    
    void TearDown() override {
        // Cleanup is handled by the destructor
    }
};

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GlobalTestEnvironment);
    return RUN_ALL_TESTS();
}
