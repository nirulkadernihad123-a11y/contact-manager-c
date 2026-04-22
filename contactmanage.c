#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CONTACTS 100
#define NAME_LEN     50
#define PHONE_LEN    20
#define EMAIL_LEN    60
#define ADDRESS_LEN  100
#define DATA_FILE    "contacts.dat"


typedef struct {
    int  id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
    char address[ADDRESS_LEN];
} Contact;


Contact contacts[MAX_CONTACTS];
int     count      = 0;
int     nextId     = 1;


void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen(void) {
    printf("\n  Press Enter to continue...");
    while (getchar() != '\n');
}


void inputLine(const char *prompt, char *dest, int maxLen) {
    printf("%s", prompt);
    if (fgets(dest, maxLen, stdin)) {
        dest[strcspn(dest, "\n")] = '\0';
    }
}

void printLine(char ch, int len) {
    for (int i = 0; i < len; i++) putchar(ch);
    putchar('\n');
}

void printHeader(const char *title) {
    clearScreen();
    printLine('=', 60);
    printf("   CONTACT MANAGEMENT SYSTEM  |  %s\n", title);
    printLine('=', 60);
}


void saveContacts(void) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) { printf("  [!] Could not save data.\n"); return; }
    fwrite(&count,    sizeof(int), 1,     fp);
    fwrite(&nextId,   sizeof(int), 1,     fp);
    fwrite(contacts,  sizeof(Contact), count, fp);
    fclose(fp);
}

void loadContacts(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return;   
    fread(&count,   sizeof(int), 1,     fp);
    fread(&nextId,  sizeof(int), 1,     fp);
    fread(contacts, sizeof(Contact), count, fp);
    fclose(fp);
}


void printContactRow(const Contact *c) {
    printf("  %-4d %-22s %-15s %s\n",
           c->id, c->name, c->phone, c->email);
}

void printContactDetail(const Contact *c) {
    printLine('-', 60);
    printf("  ID      : %d\n",  c->id);
    printf("  Name    : %s\n",  c->name);
    printf("  Phone   : %s\n",  c->phone);
    printf("  Email   : %s\n",  c->email);
    printf("  Address : %s\n",  c->address);
    printLine('-', 60);
}

void listAll(void) {
    if (count == 0) { printf("  No contacts found.\n"); return; }
    printf("  %-4s %-22s %-15s %s\n", "ID", "Name", "Phone", "Email");
    printLine('-', 60);
    for (int i = 0; i < count; i++) printContactRow(&contacts[i]);
    printf("\n  Total: %d contact(s)\n", count);
}


int findById(int id) {
    for (int i = 0; i < count; i++)
        if (contacts[i].id == id) return i;
    return -1;
}




void addContact(void) {
    printHeader("Add New Contact");

    if (count >= MAX_CONTACTS) {
        printf("  [!] Storage full (%d contacts max).\n", MAX_CONTACTS);
        pauseScreen(); return;
    }

    Contact c;
    c.id = nextId++;

    
    while (getchar() != '\n');

    inputLine("  Name    : ", c.name,    NAME_LEN);
    inputLine("  Phone   : ", c.phone,   PHONE_LEN);
    inputLine("  Email   : ", c.email,   EMAIL_LEN);
    inputLine("  Address : ", c.address, ADDRESS_LEN);

    if (strlen(c.name) == 0) {
        printf("  [!] Name cannot be empty. Contact not added.\n");
        pauseScreen(); return;
    }

    contacts[count++] = c;
    saveContacts();
    printf("\n  [✓] Contact #%d added successfully.\n", c.id);
    pauseScreen();
}


void viewAll(void) {
    printHeader("All Contacts");
    listAll();
    pauseScreen();
}


void viewDetail(void) {
    printHeader("View Contact Detail");

    if (count == 0) { printf("  No contacts stored.\n"); pauseScreen(); return; }
    listAll();

    int id;
    printf("\n  Enter ID to view (-1 to cancel): ");
    scanf("%d", &id);
    if (id == -1) return;

    int idx = findById(id);
    if (idx == -1) { printf("  [!] ID %d not found.\n", id); }
    else           { printContactDetail(&contacts[idx]); }
    pauseScreen();
}


void searchContacts(void) {
    printHeader("Search Contacts");

    while (getchar() != '\n');   

    char keyword[NAME_LEN];
    inputLine("  Enter name / phone / email keyword: ", keyword, NAME_LEN);

    
    char kl[NAME_LEN];
    for (int i = 0; keyword[i]; i++) kl[i] = tolower((unsigned char)keyword[i]);
    kl[strlen(keyword)] = '\0';

    int found = 0;
    printf("\n  %-4s %-22s %-15s %s\n", "ID", "Name", "Phone", "Email");
    printLine('-', 60);

    for (int i = 0; i < count; i++) {
        
        char n[NAME_LEN], p[PHONE_LEN], e[EMAIL_LEN];
        for (int j = 0; contacts[i].name[j];  j++) n[j] = tolower((unsigned char)contacts[i].name[j]);
        n[strlen(contacts[i].name)] = '\0';
        for (int j = 0; contacts[i].phone[j]; j++) p[j] = tolower((unsigned char)contacts[i].phone[j]);
        p[strlen(contacts[i].phone)] = '\0';
        for (int j = 0; contacts[i].email[j]; j++) e[j] = tolower((unsigned char)contacts[i].email[j]);
        e[strlen(contacts[i].email)] = '\0';

        if (strstr(n, kl) || strstr(p, kl) || strstr(e, kl)) {
            printContactRow(&contacts[i]);
            found++;
        }
    }

    if (!found) printf("  No results for \"%s\".\n", keyword);
    else        printf("\n  Found %d match(es).\n", found);
    pauseScreen();
}


void editContact(void) {
    printHeader("Edit Contact");

    if (count == 0) { printf("  No contacts to edit.\n"); pauseScreen(); return; }
    listAll();

    int id;
    printf("\n  Enter ID to edit (-1 to cancel): ");
    scanf("%d", &id);
    if (id == -1) return;

    int idx = findById(id);
    if (idx == -1) { printf("  [!] ID %d not found.\n", id); pauseScreen(); return; }

    printf("\n  Leave a field blank to keep the current value.\n");
    printContactDetail(&contacts[idx]);

    while (getchar() != '\n');   

    char buf[ADDRESS_LEN];

    inputLine("  New Name    : ", buf, NAME_LEN);
    if (strlen(buf)) strncpy(contacts[idx].name, buf, NAME_LEN - 1);

    inputLine("  New Phone   : ", buf, PHONE_LEN);
    if (strlen(buf)) strncpy(contacts[idx].phone, buf, PHONE_LEN - 1);

    inputLine("  New Email   : ", buf, EMAIL_LEN);
    if (strlen(buf)) strncpy(contacts[idx].email, buf, EMAIL_LEN - 1);

    inputLine("  New Address : ", buf, ADDRESS_LEN);
    if (strlen(buf)) strncpy(contacts[idx].address, buf, ADDRESS_LEN - 1);

    saveContacts();
    printf("\n  [✓] Contact #%d updated.\n", id);
    pauseScreen();
}


void deleteContact(void) {
    printHeader("Delete Contact");

    if (count == 0) { printf("  No contacts to delete.\n"); pauseScreen(); return; }
    listAll();

    int id;
    printf("\n  Enter ID to delete (-1 to cancel): ");
    scanf("%d", &id);
    if (id == -1) return;

    int idx = findById(id);
    if (idx == -1) { printf("  [!] ID %d not found.\n", id); pauseScreen(); return; }

    printContactDetail(&contacts[idx]);
    printf("  Confirm delete? (y/n): ");
    while (getchar() != '\n');
    char ch = getchar();
    if (ch != 'y' && ch != 'Y') { printf("  Cancelled.\n"); pauseScreen(); return; }

    for (int i = idx; i < count - 1; i++) contacts[i] = contacts[i + 1];
    count--;
    saveContacts();
    printf("\n  [✓] Contact #%d deleted.\n", id);
    pauseScreen();
}


void sortByName(void) {
    for (int i = 0; i < count - 1; i++) {
        int min = i;
        for (int j = i + 1; j < count; j++)
            if (strcasecmp(contacts[j].name, contacts[min].name) < 0) min = j;
        if (min != i) { Contact tmp = contacts[i]; contacts[i] = contacts[min]; contacts[min] = tmp; }
    }
    saveContacts();
}

void sortContacts(void) {
    printHeader("Sort Contacts");
    if (count == 0) { printf("  Nothing to sort.\n"); pauseScreen(); return; }
    sortByName();
    printf("  [✓] Contacts sorted alphabetically by name.\n");
    listAll();
    pauseScreen();
}


void showMenu(void) {
    printHeader("Main Menu");
    printf("  Contacts stored: %d / %d\n\n", count, MAX_CONTACTS);
    printf("  [1] Add Contact\n");
    printf("  [2] View All Contacts\n");
    printf("  [3] View Contact Detail\n");
    printf("  [4] Search Contacts\n");
    printf("  [5] Edit Contact\n");
    printf("  [6] Delete Contact\n");
    printf("  [7] Sort by Name\n");
    printf("  [0] Exit\n");
    printLine('-', 60);
    printf("  Choice: ");
}

int main(void) {
    loadContacts();

    int choice;
    do {
        showMenu();
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); choice = -1; }

        switch (choice) {
            case 1: addContact();    break;
            case 2: viewAll();       break;
            case 3: viewDetail();    break;
            case 4: searchContacts();break;
            case 5: editContact();   break;
            case 6: deleteContact(); break;
            case 7: sortContacts();  break;
            case 0: printf("\n  Goodbye!\n\n"); break;
            default: printf("  [!] Invalid option.\n"); pauseScreen();
        }
    } while (choice != 0);

    return 0;
}
