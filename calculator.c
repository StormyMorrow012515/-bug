#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

// 函数声明
double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double a, double b);
double squareRoot(double a);
double sine(double a);
double cosine(double a);
double tangent(double a);
double factorial(int n);
void displayMenu();
void performBasicOperation();
void performScientificOperation();

// 基础四则运算函数
double add(double a, double b) {
    return a + b;
}

double subtract(double a, double b) {
    return a - b;
}

double multiply(double a, double b) {
    return a * b;
}

double divide(double a, double b) {
    if (b == 0) {
        printf("错误：不能除以零！\n");
        return 0;
    }
    return a / b;
}

// 科学运算函数
double power(double a, double b) {
    return pow(a, b);
}

double squareRoot(double a) {
    if (a < 0) {
        printf("错误：负数没有平方根（实数范围内）！\n");
        return 0;
    }
    return sqrt(a);
}

double sine(double a) {
    // 将角度转换为弧度
    return sin(a * M_PI / 180.0);
}

double cosine(double a) {
    // 将角度转换为弧度
    return cos(a * M_PI / 180.0);
}

double tangent(double a) {
    // 将角度转换为弧度
    return tan(a * M_PI / 180.0);
}

double factorial(int n) {
    if (n < 0) {
        printf("错误：阶乘不能为负数！\n");
        return 0;
    }
    if (n == 0 || n == 1) {
        return 1;
    }
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// 显示菜单
void displayMenu() {
    printf("\n========== 科学计算器 ==========\n");
    printf("请选择操作类型：\n");
    printf("1. 基础四则运算（加、减、乘、除）\n");
    printf("2. 科学运算（幂、平方根、三角函数、阶乘）\n");
    printf("3. 退出程序\n");
    printf("================================\n");
}

// 基础运算
void performBasicOperation() {
    printf("\n--- 基础四则运算 ---\n");
    printf("1. 加法 (+)\n");
    printf("2. 减法 (-)\n");
    printf("3. 乘法 (*)\n");
    printf("4. 除法 (/)\n");
    printf("请选择操作 (1-4): ");
    
    int choice;
    scanf("%d", &choice);
    
    double num1, num2, result;
    
    printf("请输入第一个数字: ");
    scanf("%lf", &num1);
    
    printf("请输入第二个数字: ");
    scanf("%lf", &num2);
    
    switch (choice) {
        case 1:
            result = add(num1, num2);
            printf("%.2f + %.2f = %.2f\n", num1, num2, result);
            break;
        case 2:
            result = subtract(num1, num2);
            printf("%.2f - %.2f = %.2f\n", num1, num2, result);
            break;
        case 3:
            result = multiply(num1, num2);
            printf("%.2f * %.2f = %.2f\n", num1, num2, result);
            break;
        case 4:
            result = divide(num1, num2);
            if (num2 != 0) {
                printf("%.2f / %.2f = %.2f\n", num1, num2, result);
            }
            break;
        default:
            printf("无效选择！\n");
    }
}

// 科学运算
void performScientificOperation() {
    printf("\n--- 科学运算 ---\n");
    printf("1. 幂运算 (a^b)\n");
    printf("2. 平方根\n");
    printf("3. 正弦 (sin)\n");
    printf("4. 余弦 (cos)\n");
    printf("5. 正切 (tan)\n");
    printf("6. 阶乘 (n!)\n");
    printf("请选择操作 (1-6): ");
    
    int choice;
    scanf("%d", &choice);
    
    double result;
    
    switch (choice) {
        case 1: {
            double base, exponent;
            printf("请输入底数: ");
            scanf("%lf", &base);
            printf("请输入指数: ");
            scanf("%lf", &exponent);
            result = power(base, exponent);
            printf("%.2f ^ %.2f = %.2f\n", base, exponent, result);
            break;
        }
        case 2: {
            double num;
            printf("请输入数字: ");
            scanf("%lf", &num);
            result = squareRoot(num);
            if (num >= 0) {
                printf("√%.2f = %.2f\n", num, result);
            }
            break;
        }
        case 3: {
            double angle;
            printf("请输入角度（度）: ");
            scanf("%lf", &angle);
            result = sine(angle);
            printf("sin(%.2f°) = %.4f\n", angle, result);
            break;
        }
        case 4: {
            double angle;
            printf("请输入角度（度）: ");
            scanf("%lf", &angle);
            result = cosine(angle);
            printf("cos(%.2f°) = %.4f\n", angle, result);
            break;
        }
        case 5: {
            double angle;
            printf("请输入角度（度）: ");
            scanf("%lf", &angle);
            result = tangent(angle);
            printf("tan(%.2f°) = %.4f\n", angle, result);
            break;
        }
        case 6: {
            int num;
            printf("请输入非负整数: ");
            scanf("%d", &num);
            result = factorial(num);
            if (num >= 0) {
                printf("%d! = %.0f\n", num, result);
            }
            break;
        }
        default:
            printf("无效选择！\n");
    }
}

// 主函数
int main() {
    printf("欢迎使用科学计算器！\n");
    
    int mainChoice;
    
    while (1) {
        displayMenu();
        printf("请选择 (1-3): ");
        scanf("%d", &mainChoice);
        
        switch (mainChoice) {
            case 1:
                performBasicOperation();
                break;
            case 2:
                performScientificOperation();
                break;
            case 3:
                printf("感谢使用计算器！再见！\n");
                return 0;
            default:
                printf("无效选择，请重试！\n");
        }
    }
    
    return 0;
}
