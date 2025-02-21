int main(int x){
    switch(1) {
        goto abc;
        break;
    }
    abc:
        x = 1;
}