#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

/* 部分编译器（如 MSVC）需要 _USE_MATH_DEFINES 才提供 M_PI，这里兜底 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define INPUT_SIZE 256
/* 170! 是 double 能表示的最后一个有限阶乘，再大就溢出成 inf */
#define FACTORIAL_MAX 170

typedef enum {
    ANGLE_DEGREE,   /* 角度制 */
    ANGLE_RADIAN    /* 弧度制 */
} AngleUnit;

static AngleUnit g_angleUnit = ANGLE_DEGREE;
static const char *g_error = NULL;   /* 当前运算的错误信息，NULL 表示成功 */
static int g_eof = 0;                /* 标准输入是否已结束（Ctrl+D / Ctrl+Z） */
static int g_hasLast = 0;            /* 是否已有可引用的上一次结果 */
static double g_lastResult = 0.0;

/* ==================== 函数声明 ==================== */

double add(double a, double b);
double subtract(double a, double b);
double multiply(double a, double b);
double divide(double a, double b);
double power(double a, double b);
double squareRoot(double a);
double sine(double a);
double cosine(double a);
double tangent(double a);
double naturalLog(double a);
double commonLog(double a);
double exponential(double a);
double absolute(double a);
double factorial(int n);

void displayMenu();
void displayBasicMenu();
void displayScientificMenu();
void performBasicOperation();
void performScientificOperation();
void toggleAngleUnit();

/* ==================== 输入层 ==================== */

static void setError(const char *message) {
    if (g_error == NULL) {
        g_error = message;
    }
}

/* 读取一整行，去掉行尾换行；超长部分丢弃，避免残留影响下一次输入 */
static int readLine(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        g_eof = 1;
        return 0;
    }

    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    } else if (length == size - 1 && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* 丢弃超长行的剩余字符 */
        }
    }
    if (length > 0 && buffer[length - 1] == '\r') {
        buffer[length - 1] = '\0';
    }
    return 1;
}

/* 读取整数，非法输入会提示并重新读取，不会污染后续输入 */
static int readInt(const char *prompt, int *value) {
    char line[INPUT_SIZE];

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!readLine(line, sizeof(line))) {
            return 0;
        }

        char *end = NULL;
        errno = 0;
        long parsed = strtol(line, &end, 10);

        if (end == line) {
            printf("输入无效，请输入一个整数。\n");
            continue;
        }
        while (*end != '\0' && isspace((unsigned char)*end)) {
            end++;
        }
        if (*end != '\0') {
            printf("输入无效，请输入一个整数。\n");
            continue;
        }
        if (errno == ERANGE || parsed < INT_MIN || parsed > INT_MAX) {
            printf("数字超出范围，请重新输入。\n");
            continue;
        }

        *value = (int)parsed;
        return 1;
    }
}

/* 读取浮点数；输入 ans 可引用上一步结果 */
static int readDouble(const char *prompt, double *value) {
    char line[INPUT_SIZE];

    for (;;) {
        printf("%s", prompt);
        fflush(stdout);

        if (!readLine(line, sizeof(line))) {
            return 0;
        }

        if (g_hasLast && (strcmp(line, "ans") == 0 || strcmp(line, "Ans") == 0 ||
                          strcmp(line, "ANS") == 0)) {
            *value = g_lastResult;
            return 1;
        }

        char *end = NULL;
        errno = 0;
        double parsed = strtod(line, &end);

        if (end == line) {
            printf("输入无效，请输入一个数字。\n");
            continue;
        }
        while (*end != '\0' && isspace((unsigned char)*end)) {
            end++;
        }
        if (*end != '\0') {
            printf("输入无效，请输入一个数字。\n");
            continue;
        }
        if (errno == ERANGE || !isfinite(parsed)) {
            printf("数字超出范围，请重新输入。\n");
            continue;
        }

        *value = parsed;
        return 1;
    }
}

/* ==================== 基础四则运算 ==================== */

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
    if (b == 0.0) {
        setError("不能除以零");
        return NAN;
    }
    return a / b;
}

/* ==================== 科学运算 ==================== */

double power(double a, double b) {
    if (a == 0.0 && b < 0.0) {
        setError("底数为零且指数为负，结果无定义");
        return NAN;
    }
    if (a < 0.0 && b != floor(b)) {
        setError("负数的非整数次幂在实数范围内无定义");
        return NAN;
    }

    double result = pow(a, b);
    if (isnan(result)) {
        setError("幂运算超出实数范围");
        return NAN;
    }
    if (isinf(result)) {
        setError("幂运算结果溢出");
        return NAN;
    }
    return result;
}

double squareRoot(double a) {
    if (a < 0.0) {
        setError("负数没有平方根（实数范围内）");
        return NAN;
    }
    return sqrt(a);
}

/* 把用户输入的数值按当前角度单位换算成弧度 */
static double toRadians(double value) {
    return (g_angleUnit == ANGLE_DEGREE) ? value * M_PI / 180.0 : value;
}

double sine(double a) {
    return sin(toRadians(a));
}

double cosine(double a) {
    return cos(toRadians(a));
}

double tangent(double a) {
    double radians = toRadians(a);
    /* 90°、270° 等位置余弦为 0，正切无定义；留一点容差吸收浮点误差 */
    if (fabs(cos(radians)) < 1e-12) {
        setError("该角度的正切无定义");
        return NAN;
    }
    return tan(radians);
}

double naturalLog(double a) {
    if (a <= 0.0) {
        setError("对数的真数必须大于零");
        return NAN;
    }
    return log(a);
}

double commonLog(double a) {
    if (a <= 0.0) {
        setError("对数的真数必须大于零");
        return NAN;
    }
    return log10(a);
}

double exponential(double a) {
    double result = exp(a);
    if (isinf(result)) {
        setError("指数运算结果溢出");
        return NAN;
    }
    return result;
}

double absolute(double a) {
    return fabs(a);
}

double factorial(int n) {
    if (n < 0) {
        setError("阶乘不能为负数");
        return NAN;
    }
    if (n > FACTORIAL_MAX) {
        setError("阶乘结果超出双精度浮点数的表示范围（最大支持 170 的阶乘）");
        return NAN;
    }

    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* ==================== 输出 ==================== */

/* 统一输出：成功则打印表达式并记录为“上一次结果”，失败则打印错误 */
static void printResult(const char *expression, double value) {
    if (g_error != NULL) {
        printf("错误：%s！\n", g_error);
        return;
    }

    printf("%s = %.10g\n", expression, value);
    g_lastResult = value;
    g_hasLast = 1;
}

static const char *angleUnitName(void) {
    return (g_angleUnit == ANGLE_DEGREE) ? "度" : "弧度";
}

static const char *angleUnitSymbol(void) {
    return (g_angleUnit == ANGLE_DEGREE) ? "°" : " rad";
}

/* 二元运算的通用流程：读两个数 -> 计算 -> 输出 */
static void performBinaryOperation(const char *symbol, double (*operation)(double, double)) {
    double a = 0.0;
    double b = 0.0;

    if (!readDouble("请输入第一个数字: ", &a)) {
        return;
    }
    if (!readDouble("请输入第二个数字: ", &b)) {
        return;
    }

    g_error = NULL;
    double result = operation(a, b);

    char expression[128];
    snprintf(expression, sizeof(expression), "%.10g %s %.10g", a, symbol, b);
    printResult(expression, result);
}

/* 三角函数通用流程：按当前角度单位读取角度 */
static void performTrigOperation(const char *name, double (*operation)(double)) {
    char prompt[64];
    snprintf(prompt, sizeof(prompt), "请输入角度（%s）: ", angleUnitName());

    double angle = 0.0;
    if (!readDouble(prompt, &angle)) {
        return;
    }

    g_error = NULL;
    double result = operation(angle);

    char expression[128];
    snprintf(expression, sizeof(expression), "%s(%.10g%s)", name, angle, angleUnitSymbol());
    printResult(expression, result);
}

/* ==================== 菜单 ==================== */

void displayMenu() {
    printf("\n========== 科学计算器 ==========\n");
    printf("1. 基础四则运算（加、减、乘、除）\n");
    printf("2. 科学运算（幂、开方、三角、对数、阶乘等）\n");
    printf("3. 切换角度单位（当前：%s）\n", angleUnitName());
    printf("4. 退出程序\n");
    printf("================================\n");
}

void displayBasicMenu() {
    printf("\n--- 基础四则运算 ---\n");
    printf("1. 加法 (+)\n");
    printf("2. 减法 (-)\n");
    printf("3. 乘法 (*)\n");
    printf("4. 除法 (/)\n");
    printf("0. 返回上一级\n");
}

void displayScientificMenu() {
    printf("\n--- 科学运算 ---\n");
    printf(" 1. 幂运算 (a^b)\n");
    printf(" 2. 平方根 (√x)\n");
    printf(" 3. 正弦 sin\n");
    printf(" 4. 余弦 cos\n");
    printf(" 5. 正切 tan\n");
    printf(" 6. 阶乘 (n!)\n");
    printf(" 7. 自然对数 ln\n");
    printf(" 8. 常用对数 log10\n");
    printf(" 9. 自然指数 e^x\n");
    printf("10. 绝对值 |x|\n");
    printf(" 0. 返回上一级\n");
}

void toggleAngleUnit() {
    g_angleUnit = (g_angleUnit == ANGLE_DEGREE) ? ANGLE_RADIAN : ANGLE_DEGREE;
    printf("已切换为%s制。\n", angleUnitName());
}

/* ==================== 具体操作 ==================== */

void performBasicOperation() {
    displayBasicMenu();

    int choice = -1;
    if (!readInt("请选择操作 (0-4): ", &choice)) {
        return;
    }

    switch (choice) {
        case 1:
            performBinaryOperation("+", add);
            break;
        case 2:
            performBinaryOperation("-", subtract);
            break;
        case 3:
            performBinaryOperation("*", multiply);
            break;
        case 4:
            performBinaryOperation("/", divide);
            break;
        case 0:
            break;
        default:
            printf("无效选择！\n");
            break;
    }
}

void performScientificOperation() {
    displayScientificMenu();

    int choice = -1;
    if (!readInt("请选择操作 (0-10): ", &choice)) {
        return;
    }

    switch (choice) {
        case 1:
            performBinaryOperation("^", power);
            break;
        case 2: {
            double value = 0.0;
            if (!readDouble("请输入数字: ", &value)) {
                return;
            }
            g_error = NULL;
            double result = squareRoot(value);
            char expression[128];
            snprintf(expression, sizeof(expression), "√%.10g", value);
            printResult(expression, result);
            break;
        }
        case 3:
            performTrigOperation("sin", sine);
            break;
        case 4:
            performTrigOperation("cos", cosine);
            break;
        case 5:
            performTrigOperation("tan", tangent);
            break;
        case 6: {
            int n = 0;
            if (!readInt("请输入非负整数: ", &n)) {
                return;
            }
            g_error = NULL;
            double result = factorial(n);
            char expression[64];
            snprintf(expression, sizeof(expression), "%d!", n);
            printResult(expression, result);
            break;
        }
        case 7: {
            double value = 0.0;
            if (!readDouble("请输入真数: ", &value)) {
                return;
            }
            g_error = NULL;
            double result = naturalLog(value);
            char expression[128];
            snprintf(expression, sizeof(expression), "ln(%.10g)", value);
            printResult(expression, result);
            break;
        }
        case 8: {
            double value = 0.0;
            if (!readDouble("请输入真数: ", &value)) {
                return;
            }
            g_error = NULL;
            double result = commonLog(value);
            char expression[128];
            snprintf(expression, sizeof(expression), "log10(%.10g)", value);
            printResult(expression, result);
            break;
        }
        case 9: {
            double value = 0.0;
            if (!readDouble("请输入指数: ", &value)) {
                return;
            }
            g_error = NULL;
            double result = exponential(value);
            char expression[128];
            snprintf(expression, sizeof(expression), "e^%.10g", value);
            printResult(expression, result);
            break;
        }
        case 10: {
            double value = 0.0;
            if (!readDouble("请输入数字: ", &value)) {
                return;
            }
            g_error = NULL;
            double result = absolute(value);
            char expression[128];
            snprintf(expression, sizeof(expression), "|%.10g|", value);
            printResult(expression, result);
            break;
        }
        case 0:
            break;
        default:
            printf("无效选择！\n");
            break;
    }
}

/* ==================== 主函数 ==================== */

int main(void) {
    printf("欢迎使用科学计算器！\n");
    printf("提示：在需要输入数字的地方输入 ans，可引用上一次的计算结果。\n");

    while (!g_eof) {
        displayMenu();

        int choice = -1;
        if (!readInt("请选择 (1-4): ", &choice)) {
            break;
        }

        switch (choice) {
            case 1:
                performBasicOperation();
                break;
            case 2:
                performScientificOperation();
                break;
            case 3:
                toggleAngleUnit();
                break;
            case 4:
                printf("感谢使用计算器！再见！\n");
                return 0;
            default:
                printf("无效选择，请重试！\n");
                break;
        }
    }

    printf("\n感谢使用计算器！再见！\n");
    return 0;
}
