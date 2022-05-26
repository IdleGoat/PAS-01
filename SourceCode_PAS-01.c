#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

/*
Proyek Akhir Pemrograman Lanjut :

Restaurant Management System

Rafie Amandio Fauzan (2106731232)
Bintang Marsyuma Rakhasunu (2106731415)

*/

// Struct Akun Manager
typedef struct account{
	char nama[100];
	char pass[100];
	char first[100];
	char last[100];
}account;

// Struct Linkedlist Pesanan
typedef struct order{
	int order[20];
	char nama[100];
	int jenis;
	double total;
	struct order *next;
}order;

//Struct Rekap order
typedef struct history{
	int totalorder[20];
	double totalharga;
	double totalcash;
	double totalqris;
	double totalbank;
}history;

//Struct Menu
typedef struct foodmenu{
	char nama[100];
	int jenis;
	double harga;
}foodmenu;

int acc_count = 0;
int menu_count = 0;

//Prototype Function
void title(char string[20]);
void loadmenu(foodmenu *menulist);
void print(order **head,foodmenu *menulist);
void printmenu(foodmenu *menulist);
void deleteNode(order** head_ref, int key);
void push(order temp,order **head);
void daftar(account *karyawan);
void ordermasuk(order **head,foodmenu *menulist);
void loadakun(account *karyawan);
void login(account *karyawan,order **head,foodmenu *menulist,history *riwayat);
void menu(account *karyawan,int login,order **head,foodmenu *menulist,history *riwayat);
void deltengah(order **head,int pos);
void quickSort(int *data,int *arr, int low, int high);
void swap(int *arr,int i,int j);
void reverseArray(int arr[], int start, int end);



int main(void){
	
	int counter,choice,i;
	
	history riwayat;
	account karyawan[15];
	foodmenu menulist[20];
	loadmenu(menulist); // Function Load menu dari file
	loadakun(karyawan); // Function Load akun dari file
	Sleep(2000);
	system("cls");
	order *head = NULL; // Pembuatan Head Linkedlist
	omp_set_num_threads(omp_get_max_threads());
	while(1){
		title("Menu Akun");
		
		printf( "\n\t\t1. Login Akun\n\t\t2. Daftar Akun");
		printf("\n\t\tMasukkan Pilihan Menu : ");
		scanf("%d",&choice);
		switch(choice){
			case 1 :
				// Function Login
				login(karyawan,&head,menulist,&riwayat);
				break;
			case 2 :
				//Function Pembuatan Akun
				daftar(karyawan);
				break;
			default :
				break;
	}
		
	}
	
}

void printlist(order **head,foodmenu *menulist){ // Function print linkedlist
	int i,count=1,ordcount=1;
	order *n = *head;
	char jenis[3][20]={"Cash","Qris","Bank"};
	if(head == NULL){  
        printf("\nList kosong");  
    }
	else{
		while (n != NULL) {
		count=1;
		printf("\n===========================%d=========================",ordcount);
        printf("\nNama Pemesan\t\t: %s", n->nama);
        printf("\nJenis Pembayaran\t: %s",jenis[(n->jenis)-1]);
        printf("\nPesanan : ");
        for(i=0;i<20;i++){ //Looping print pesanan
        	if(n->order[i]){
        		printf("\n%d. %s\t\tx%d",count,menulist[i].nama,n->order[i]);
        		count++;
			}
		}
		printf("\nHarga Total\t\t: Rp%lf\n",n->total);
		printf("=====================================================\n");
        ordcount++;
        n = n->next;
    	}
	} 
}

void push(order temp,order **head){ // Push data input ke dalam linkedlist
	int i;
	order *nodebaru = (order*)malloc(sizeof(order)); // pembuatan node baru
	order *last = *head;
	if(nodebaru == NULL){  
        printf("\nOVERFLOW");
        return;
    }
    strcpy(nodebaru->nama,temp.nama);
    nodebaru->jenis = temp.jenis;
    nodebaru->total = temp.total;
    for(i=0;i<20;i++){
    	nodebaru->order[i] = temp.order[i];
	}
    nodebaru->next = NULL;
    
    if(*head == NULL){
    	*head = nodebaru;
    	return;
	}
	
	while(last->next != NULL){
		last = last->next;
	}
    
    last->next = nodebaru;
}

void ordermasuk(order **head,foodmenu *menulist){ //Function untuk menerima order baru
	double sum;
	int choice,quantity,i;
	order temp;
	title("Input Order");
	printf("\nMasukkan Nama Pemesan : ");
	scanf(" %99[^\n]s",&temp.nama);
	printf("Masukkan Jenis Pembayaran (1: Cash, 2: Qris, 3.Bank) : ");
	scanf("%d",&temp.jenis);
	printf("\nMasukkan Menu yang ingin dibeli\n");
	
	for(i=0;i<menu_count;i++){
			printf("%d. %s\n",i+1,menulist[i].nama);
	}
	// parallel 
	#pragma omp parallel for 
	for(i=0;i<20;i++){
		
		#pragma omp critical
		temp.order[i]=0;
	}
	
	while(1){
		printf("\nMasukkan Pilihan Menu (Masukkan 0 Untuk mengakhiri): ");
		scanf("%d",&choice);
		if(choice == 0){
			break;
		}
		printf("Kuantitas : ");
		scanf("%d",&quantity);
		temp.order[choice-1] = quantity;
	}
	
	#pragma omp parallel for reduction( + : sum)
	for(i=0;i<20;i++){
		sum += temp.order[i] * menulist[i].harga;
	}
		
	temp.total = sum;
	
	push(temp,head); //memasukkan data ke linked list
		
}

void recapprint(history *riwayat,foodmenu *menulist,account *karyawan,int login){
	int i,urutan[menu_count],temp[menu_count],choice;
	time_t tm;
    time(&tm);
    FILE *fpr;
    fpr = fopen("report.txt","w");
	double tax,sum;
	for(i=0;i<menu_count;i++){
		urutan[i] = i;
		temp[i] = riwayat->totalorder[i];
	}
	
	// parallel region untuk sorting
	#pragma omp parallel
	{
		printf("Parallel Quicksort with thread %d\n",omp_get_thread_num());
		#pragma omp barrier // barrier agar printf dijalankan semua terlebih dahulu
		#pragma omp single
		{
			//Call Function Sort
			quickSort(urutan,temp,0,menu_count-1);
		}
	}
	// array
	reverseArray(urutan, 0, menu_count-1);
	
	title("Rekapitulasi");
	printf("\nWaktu Pembuatan Laporan : %s",ctime(&tm));
	printf("\nPenanggung Jawab : %s",karyawan[login].nama);
	printf("\n=====================================================\n");
	printf("  Menu\t\t\tJumlah\tHarga");
	printf("\n=====================================================\n");
	for(i=0;i<menu_count;i++){
		sum = menulist[urutan[i]].harga * riwayat->totalorder[urutan[i]];
		printf("\n%d. %s\t%d\tRp %.0lf",i+1,menulist[urutan[i]].nama,riwayat->totalorder[urutan[i]],sum);
	}
	
	printf("\n=====================================================");
	printf("\n Penjualan Cash\t\t:Rp %.02lf",riwayat->totalcash);
	printf("\n Penjualan Qris\t\t:Rp %.02lf",riwayat->totalqris);
	printf("\n Penjualan Bank\t\t:Rp %.02lf",riwayat->totalbank);
	printf("\n=====================================================\n");
	printf("\n Penjualan Total\t:Rp %.02lf",riwayat->totalharga);
	tax = (riwayat->totalharga)*11/100;
	printf("\n PPN 10%%\t\t:Rp. %.02lf",tax);
	printf("\n Keuntungan Bersih\t:Rp %.02lf",(riwayat->totalharga)-tax);
	printf("\n=====================================================\n");
	
	printf("Simpan Rekap Laporan ? (1/0) : ");
	scanf("%d",&choice);
	
	if(choice){
		fprintf(fpr,"\nWaktu Pembuatan Laporan : %s",ctime(&tm));
		fprintf(fpr,"\nPenanggung Jawab : %s",karyawan[login].nama);
		fprintf(fpr,"\n=====================================================\n");
		fprintf(fpr,"  Menu\t\t\tJumlah\tHarga");
		fprintf(fpr,"\n=====================================================\n");
		for(i=0;i<menu_count;i++){
			sum = menulist[urutan[i]].harga * riwayat->totalorder[urutan[i]];
			fprintf(fpr,"\n%d. %s\t%d\tRp %.0lf",i+1,menulist[urutan[i]].nama,riwayat->totalorder[urutan[i]],sum);
		}
	
		fprintf(fpr,"\n=====================================================");
		fprintf(fpr,"\n Penjualan Cash\t\t:Rp %.02lf",riwayat->totalcash);
		fprintf(fpr,"\n Penjualan Qris\t\t:Rp %.02lf",riwayat->totalqris);
		fprintf(fpr,"\n Penjualan Bank\t\t:Rp %.02lf",riwayat->totalbank);
		fprintf(fpr,"\n=====================================================\n");
		fprintf(fpr,"\n Penjualan Total\t:Rp %.02lf",riwayat->totalharga);
		fprintf(fpr,"\n PPN 11%%\t\t:Rp. %.02lf",tax);
		fprintf(fpr,"\n Keuntungan Bersih\t:Rp %.02lf",(riwayat->totalharga)-tax);
		fprintf(fpr,"\n=====================================================\n");
		fclose(fpr);
	}
	else{
		return;
	}
	
	
}




void recap(order **head,history *riwayat){
	int i;
	order *last = *head;
	
	
	if(*head == NULL){  
        printf("\nlist is empty");
        return;
    }
    
    while(last != NULL){
    	if(last->next == NULL){
    		riwayat->totalharga += last->total;
    		for(i=0;i<20;i++){
    			riwayat->totalorder[i] += last->order[i];
			}
    		switch(last->jenis){
    			case 1 : riwayat->totalcash += last->total;
    					break;
    			case 2 : riwayat->totalqris += last->total;
    					break;
    			case 3 : riwayat->totalbank += last->total;
    					break;
    			default : printf("\nUnknown Payment Type!!");
    					break;
			}
		}
    	
		last = last->next;
	}
      
}

void menu(account *karyawan,int login,order **head,foodmenu *menulist,history *riwayat){
	int choice,del,i;
	riwayat->totalharga = 0;
	riwayat->totalcash = 0;
	riwayat->totalqris = 0;
	riwayat->totalbank = 0;
	
	for(i=0;i<20;i++){
    	riwayat->totalorder[i] = 0;
	}
	while(choice != 6){
		system("cls");
		title("Menu");
		printf("======================================================\n");
		printf("Penanggung Jawab : %s",karyawan[login].nama);
		printf("\n======================================================\n");
		printlist(head,menulist);
		printf( " \n\t\t1. Input Order\n\t\t2. Finish Order  \n\t\t3. Daftar Order\n\t\t4. Daftar Menu\n\t\t5. Rekap Penjualan");
		printf("\n\t\tMasukkan Pilihan Menu : ");
		scanf("%d",&choice);
		switch(choice){
			case 1 :
				ordermasuk(head,menulist);
				recap(head,riwayat);
				system("pause");
				break;
			case 2 :
				printf("\n Masukkan Nomor Order Yang Telah Selesai :");
				scanf("%d",&del);
				deltengah(head,del);
				system("pause");
				break;
			case 3 :
				printlist(head,menulist);
				system("pause");
				break;
			case 4 :
				system("cls");
				printmenu(menulist);
				system("pause");
				break;
			case 5 :
				recapprint(riwayat,menulist,karyawan,login);
				system("pause");
				break;
			case 6 :
				break;
		
		}		
	}
}

void deltengah(order **head,int pos){
	int i;
	if(*head == NULL){  
        printf("\nlist is empty");
        return;
    }
    order *temp = *head;
    if(pos == 1){
    	*head = temp->next;
    	free(temp);
    	return;
	}
	
	for(i=2;i<pos && temp != NULL;i++){
		temp = temp->next;
		
		if(temp == NULL || temp->next == NULL){
			return;
		}
	}
	
	order *del = temp->next;
	temp->next = del->next;
	del->next = NULL;
	free(del);
}

void printmenu(foodmenu *menulist){
	int i;
	title("List Menu");
	char jenis[2][20]={"Makanan","Minuman"};
	printf("No.\tNama\t\t\tJenis\t\tHarga\n");
	for(i=0;i<menu_count;i++){
		printf("%d\t%.20s\t\t%s\t\tRp %.02lf\n",i+1,menulist[i].nama,jenis[menulist[i].jenis],menulist[i].harga);
	}
}


void login(account *karyawan,order **head,foodmenu *menulist,history *riwayat){
	char user[100],pass[100],c;
	int i,j,status=0,login;
	
	while(!status){
		j=0;
		title("Login");
		printf("Username: ");
    	scanf(" %99[^\n]s", &user);
    	printf("Password: ");
    	while(j<99){
	    	pass[j]=getch();
	    	c=pass[j];
	    	if(c==13) break;
	    	else printf("*");
	    	j++;
		}
		pass[j]='\0';
		
		#pragma omp parallel
		{
			
		#pragma omp for
    	for(i=0;i<acc_count;i++){
    		#pragma omp critical
    		if(strcmp(karyawan[i].nama,user) == 0 && strcmp(karyawan[i].pass,pass) == 0 ){
    				login = i;
    				status = 1;
			}
		}
		}
		
		if(status == 1){
			printf("\nLogin Sukses!!",karyawan[login].nama);
			Sleep(2000);
			menu(karyawan,login,head,menulist,riwayat);
		}
		else{
			printf("\nGagal Login\n\n");
		}
		
	}
	
	
    
}

void loadakun(account *karyawan){
	char *token;
  	char buffer[1024];
	FILE *fp;
  	int datacount=0,col,i;
	
	fp = fopen("akun.txt","r");
	
	if(fp == NULL){ // Error Correction apabila tidak terdapat file
		printf("Tidak Terdapat File Akun untuk Di Load !!\n");
		system("pause");
		return;
	}
	// Loop untuk mengambil data dari file csv
	
	while (fgets(buffer, sizeof(buffer), fp)){
		if(datacount >= 15){
			printf("ERROR : Data Melebihi 15!!");
			break;
		}
		col =1;

    	token = strtok(buffer, ";");
    	while(token != NULL){
   		// Mengambil nama dalam kolom pertama
    	if(col == 1){
    		strcpy(karyawan[acc_count].nama,token);
		}
		// Mengambil Asal Negara dari kolom kedua
		else if(col ==2){
			strcpy(karyawan[acc_count].pass,token);
		}
		col++;
    	token = strtok(NULL, ";");
    	}
    	acc_count++;
    	
	}
		printf("\n============================================\n");	
		printf("|      Data Akun telah sukses diload!!     |\n");
		printf("============================================");	
		fclose(fp);
	
}

void loadmenu(foodmenu *menulist){
	char *token;
  	char buffer[1024];
  	char *ptr;
	FILE *fp;
  	int datacount=0,col,i;
  	char type[2][30] = {"Makanan","Minuman"};
	
	fp = fopen("menu.csv","r");
	
	if(fp == NULL){ // Error Correction apabila tidak terdapat file
		printf("Tidak Terdapat File untuk Di Load !!\n");
		system("pause");
		return;
	}
	// Loop untuk mengambil data dari file csv
	
	while (fgets(buffer, sizeof(buffer), fp)){
		if(datacount >= 20){
			printf("ERROR : Data Melebihi 20!!");
			break;
		}
		col =1;

    	token = strtok(buffer, ";");
    	while(token != NULL){
   		// Mengambil nama dalam kolom pertama
    	if(col == 1){
    		strcpy(menulist[menu_count].nama,token);
		}
		// Mengambil Jenis dalam kolom dua
		else if(col == 2){
			menulist[menu_count].jenis = atoi(token);
		}
		// Mengambil harga dari kolom tiga
		else if(col == 3){
			menulist[menu_count].harga = strtod(token,&ptr);
		}
		col++;
    	token = strtok(NULL, ";");
    	}
    	menu_count++;
    	
	}
		
		printf("\n============================================\n");	
		printf("|      Data Menu telah sukses diload!!     |\n");
		printf("============================================\n");
		fclose(fp);
}


void daftar(account *karyawan){
	
	FILE *fp;
	title("Sign-Up");
    printf("\nNama Manajer : " );
    scanf(" %99[^\n]s", karyawan[acc_count].nama);
    printf("\nPassword : " );
    scanf(" %99[^\n]s", karyawan[acc_count].pass);
	
	fp = fopen("akun.txt","a");
	fprintf(fp,"%s;%s;\n",karyawan[acc_count].nama,karyawan[acc_count].pass);
	fclose(fp);
	acc_count++;
	 	
	
}



void title(char string[20]){
	//system("COLOR OF");
	system("cls");
	printf("\n======================================================\n"
		    "|            Restaurant Management System            |\n"
			"======================================================\n"
			"                      %s                              \n"
			"======================================================\n",string);


}

void quickSort(int *data,int *arr, int low, int high) {
  	int piv, i, j;
  	
  	if(low < high) {
    	piv = low;

    	i = low;
    	j = high;
    	
    	while(i < j) {
    		while(arr[i] <= arr[piv] && i <= high){
    			i++;
			}
    			
    	  	while(arr[j] > arr[piv] && j >= low){
    	  		j--;	
			}
    	    	
    	  	if(i < j) {
    	  		swap(arr,j,i);
				swap(data,j,i);		
      		}
    	}
    	swap(arr,j,piv);
    	swap(data,j,piv);
    	
    	#pragma omp task 
    	{
    		quickSort(data,arr, low, j-1);
		}
			
		#pragma omp task
		{
			quickSort(data,arr, j+1, high);
		}
					
		#pragma omp taskwait
  	}
}

void swap(int *arr,int i,int j){
	int temp;
	temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void reverseArray(int arr[], int start, int end){
    int temp;
    while (start < end)
    {
        temp = arr[start];  
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }  
} 
