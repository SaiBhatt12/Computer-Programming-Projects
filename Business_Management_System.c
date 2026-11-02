#include <stdio.h>
#include <string.h>

#define MAX 500
#define LEN 100

struct Record {
    int id;
    char division[40];
    char date[15];
    char description[LEN];
    float amount;
};

struct Record sales[MAX];
struct Record expenses[MAX];
int sales_count = 0;
int expenses_count = 0;

char sales_divs[4][30] = {
    "Product Sales",
    "YouTube Income",
    "Advertisement Income",
    "Other Income"
};

char expense_divs[4][30] = {
    "Raw Materials Expense",
    "Transportation Expenses",
    "Wages / Salary",
    "Other Expenses"
};

/* ===================== FILE OPERATIONS ===================== */

void load_data() {
    FILE *fp;
    char line[400];

    fp = fopen("sales.txt", "r");
    sales_count = 0;
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL && sales_count < MAX) {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) == 0) continue;
            sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%f",
                   &sales[sales_count].id,
                   sales[sales_count].division,
                   sales[sales_count].date,
                   sales[sales_count].description,
                   &sales[sales_count].amount);
            sales_count++;
        }
        fclose(fp);
    }

    fp = fopen("expenses.txt", "r");
    expenses_count = 0;
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL && expenses_count < MAX) {
            line[strcspn(line, "\n")] = '\0';
            if (strlen(line) == 0) continue;
            sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%f",
                   &expenses[expenses_count].id,
                   expenses[expenses_count].division,
                   expenses[expenses_count].date,
                   expenses[expenses_count].description,
                   &expenses[expenses_count].amount);
            expenses_count++;
        }
        fclose(fp);
    }
}

void save_sales() {
    FILE *fp = fopen("sales.txt", "w");
    if (fp == NULL) return;
    for (int i = 0; i < sales_count; i++) {
        fprintf(fp, "%d|%s|%s|%s|%.2f\n",
                sales[i].id, sales[i].division,
                sales[i].date, sales[i].description,
                sales[i].amount);
    }
    fclose(fp);
}

void save_expenses() {
    FILE *fp = fopen("expenses.txt", "w");
    if (fp == NULL) return;
    for (int i = 0; i < expenses_count; i++) {
        fprintf(fp, "%d|%s|%s|%s|%.2f\n",
                expenses[i].id, expenses[i].division,
                expenses[i].date, expenses[i].description,
                expenses[i].amount);
    }
    fclose(fp);
}

/* ===================== HELPER ===================== */

int get_next_id(int section) {
    int max_id = (section == 1) ? 1000 : 2000;
    int n = (section == 1) ? sales_count : expenses_count;
    struct Record *arr = (section == 1) ? sales : expenses;
    for (int i = 0; i < n; i++) {
        if (arr[i].id > max_id) max_id = arr[i].id;
    }
    return max_id + 1;
}

/* ===================== CRUD OPERATIONS ===================== */

void add_record(int section, char division[]) {
    struct Record *arr = (section == 1) ? sales : expenses;
    int *cnt = (section == 1) ? &sales_count : &expenses_count;

    if (*cnt >= MAX) {
        printf("Storage full!\n");
        return;
    }

    int idx = *cnt;
    arr[idx].id = get_next_id(section);
    strcpy(arr[idx].division, division);

    printf("\n--- Add Record to [%s] ---\n", division);
    printf("Enter Date (YYYY-MM-DD): ");
    fgets(arr[idx].date, 15, stdin);
    arr[idx].date[strcspn(arr[idx].date, "\n")] = '\0';

    printf("Enter Description: ");
    fgets(arr[idx].description, LEN, stdin);
    arr[idx].description[strcspn(arr[idx].description, "\n")] = '\0';

    printf("Enter Amount: ");
    scanf("%f", &arr[idx].amount);
    getchar();

    (*cnt)++;

    if (section == 1) save_sales();
    else save_expenses();

    printf("Record added! (ID: %d)\n", arr[idx].id);
}

void view_records(int section, char division[]) {
    struct Record *arr = (section == 1) ? sales : expenses;
    int n = (section == 1) ? sales_count : expenses_count;

    printf("\n--- %s ---\n", division);
    printf("%-6s %-12s %-30s %-12s\n", "ID", "Date", "Description", "Amount");
    printf("--------------------------------------------------------------\n");

    int found = 0;
    float total = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(arr[i].division, division) == 0) {
            printf("%-6d %-12s %-30s %-12.2f\n",
                   arr[i].id, arr[i].date,
                   arr[i].description, arr[i].amount);
            total += arr[i].amount;
            found = 1;
        }
    }

    if (!found) printf("No records found.\n");
    else {
        printf("--------------------------------------------------------------\n");
        printf("Total: %.2f\n", total);
    }
}

void update_record(int section, char division[]) {
    struct Record *arr = (section == 1) ? sales : expenses;
    int n = (section == 1) ? sales_count : expenses_count;

    int id;
    printf("\nEnter Record ID to update: ");
    scanf("%d", &id);
    getchar();

    int found = -1;
    for (int i = 0; i < n; i++) {
        if (arr[i].id == id && strcmp(arr[i].division, division) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Record not found in this division!\n");
        return;
    }

    printf("\nCurrent: %s | %s | %.2f\n",
           arr[found].date, arr[found].description, arr[found].amount);

    char temp[LEN];

    printf("New Date (Enter to skip): ");
    fgets(temp, 15, stdin);
    temp[strcspn(temp, "\n")] = '\0';
    if (strlen(temp) > 0) strcpy(arr[found].date, temp);

    printf("New Description (Enter to skip): ");
    fgets(temp, LEN, stdin);
    temp[strcspn(temp, "\n")] = '\0';
    if (strlen(temp) > 0) strcpy(arr[found].description, temp);

    printf("New Amount (-1 to skip): ");
    float a;
    scanf("%f", &a);
    if (a >= 0) arr[found].amount = a;
    getchar();

    if (section == 1) save_sales();
    else save_expenses();

    printf("Record updated!\n");
}

void delete_record(int section, char division[]) {
    struct Record *arr = (section == 1) ? sales : expenses;
    int *cnt = (section == 1) ? &sales_count : &expenses_count;

    int id;
    printf("\nEnter Record ID to delete: ");
    scanf("%d", &id);
    getchar();

    int found = -1;
    for (int i = 0; i < *cnt; i++) {
        if (arr[i].id == id && strcmp(arr[i].division, division) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Record not found in this division!\n");
        return;
    }

    printf("Deleting: %s | %s | %.2f\n",
           arr[found].date, arr[found].description, arr[found].amount);

    for (int i = found; i < *cnt - 1; i++) {
        arr[i] = arr[i + 1];
    }
    (*cnt)--;

    if (section == 1) save_sales();
    else save_expenses();

    printf("Record deleted!\n");
}

/* ===================== MENUS ===================== */

void division_menu(int section, char division[]) {
    int choice;
    while (1) {
        printf("\n===== %s =====\n", division);
        printf("1. Add Record\n");
        printf("2. View Records\n");
        printf("3. Update Record\n");
        printf("4. Delete Record\n");
        printf("5. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) add_record(section, division);
        else if (choice == 2) view_records(section, division);
        else if (choice == 3) update_record(section, division);
        else if (choice == 4) delete_record(section, division);
        else if (choice == 5) return;
        else printf("Invalid choice!\n");
    }
}

void section_menu(int section) {
    char (*divs)[30] = (section == 1) ? sales_divs : expense_divs;
    char *title = (section == 1) ? "SALES SECTION" : "EXPENSES SECTION";
    int choice;

    while (1) {
        printf("\n========= %s =========\n", title);
        for (int i = 0; i < 4; i++) {
            printf("%d. %s\n", i + 1, divs[i]);
        }
        printf("5. Back\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice >= 1 && choice <= 4) {
            division_menu(section, divs[choice - 1]);
        } else if (choice == 5) {
            return;
        } else {
            printf("Invalid choice!\n");
        }
    }
}

void view_summary() {
    float total_sales = 0;
    float total_expenses = 0;

    printf("\n============== SUMMARY ==============\n");

    printf("\n--- SALES ---\n");
    for (int d = 0; d < 4; d++) {
        float sub = 0;
        for (int i = 0; i < sales_count; i++) {
            if (strcmp(sales[i].division, sales_divs[d]) == 0)
                sub += sales[i].amount;
        }
        printf("  %-25s %.2f\n", sales_divs[d], sub);
        total_sales += sub;
    }
    printf("  %-25s %.2f\n", "TOTAL SALES:", total_sales);

    printf("\n--- EXPENSES ---\n");
    for (int d = 0; d < 4; d++) {
        float sub = 0;
        for (int i = 0; i < expenses_count; i++) {
            if (strcmp(expenses[i].division, expense_divs[d]) == 0)
                sub += expenses[i].amount;
        }
        printf("  %-25s %.2f\n", expense_divs[d], sub);
        total_expenses += sub;
    }
    printf("  %-25s %.2f\n", "TOTAL EXPENSES:", total_expenses);

    printf("\n--- PROFIT / LOSS ---\n");
    float profit = total_sales - total_expenses;
    if (profit >= 0)
        printf("  Net Profit: %.2f\n", profit);
    else
        printf("  Net Loss: %.2f\n", profit * -1);

    printf("=====================================\n");
}

/* ===================== MAIN ===================== */

int main() {
    load_data();
    int choice;

    while (1) {
        printf("\n=============================\n");
        printf("  BUSINESS MANAGEMENT SYSTEM\n");
        printf("=============================\n");
        printf("1. Sales Section\n");
        printf("2. Expenses Section\n");
        printf("3. View Summary\n");
        printf("4. Exit\n");
        printf("=============================\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) section_menu(1);
        else if (choice == 2) section_menu(2);
        else if (choice == 3) view_summary();
        else if (choice == 4) {
            printf("Goodbye!\n");
            return 0;
        } else {
            printf("Invalid choice!\n");
        }
    }
    return 0;
}