struct book{
    char title[50];
    char author[50];
    int year;
};
int test(char* fmt){
    return 1;
}
struct book book;
int main(){
    struct book* books;
    return test(56, books[0].title, 1000);
}