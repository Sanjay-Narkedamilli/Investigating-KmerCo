#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "prime.h"
//#include "murmur.h"
#include "KmerCo.h" //KmerCo header file
#include <sys/mman.h>
#include<fcntl.h>
#include "initBF.h"
#include "keyBF.h"
static unsigned long int TP=0,TN=0,FP=0;

unsigned long int **aBF;
unsigned long int **distinct_robustBF;
unsigned long int **trustworthy_robustBF;
unsigned long int **erroneous_robustBF;

//#### Insertion without file writing (Canonical) ##########
void insertion_canonical_without_filewrite(char fname[6][100],int kmer_len,int threshold, int k){
	int result,kcount=0,rcount=0;
    char ch,kmer[kmer_len+1],rev_kmer[kmer_len+1];	
	double err=0.001;
	unsigned long int m,r;
	double fp=0.0;
	clock_t start, end;
	unsigned long int h1,h2;
	long long int count,fileLength,total_kmers;
	FILE *fkmer,*fres;
	printf("Insertion process\n");
    int f = open(fname[0], O_RDONLY);
    if (f ==-1)
        printf("Error in opening file\n");  

	char cmd[150]="wc -c <", *s1=" > linecount.txt"; //Do not change the value of the string
	strcat(cmd,fname[0]);
	strcat(cmd,s1);
    system(cmd); // calculate #characters and save in result.txt
    
	fkmer=fopen("linecount.txt","r");
	if(fkmer==NULL){
		printf("File can't be created\n");
		exit(0);
	}
	fscanf(fkmer,"%lld",&fileLength); 
    
    printf("File length: %lld\n",(long long)fileLength); 
	total_kmers=fileLength-kmer_len; //Total number of kmers in the file
    printf("No of kmers: %lld\n",(long long)total_kmers); //print
	fclose(fkmer);

	printf("File initiated!\n");	// Filter construction
	int mem=(int)(total_kmers);
	m=memory(mem,err);
	printf("Memory initiated!\n");
	setDim(m);
	printf("Dimensions initiated!\n");
	aBF=allocate();
	printf("Filters are created!\n");

    char* buff = mmap(NULL, fileLength, PROT_READ, MAP_SHARED, f, 0); //Map its contents into memory.

    off_t bindex=0;	
	count=1; 
	kmer[kmer_len]='\0';
    rev_kmer[kmer_len]='\0';
	start=clock(); //Starting clock
    for(kcount=0,rcount=kmer_len-1;kcount<kmer_len;kcount++,rcount--){ //Inserting first kmer
        ch=buff[bindex++]; //printf("ch: %c\n",ch);
		kmer[kcount]=ch;
        switch(ch){
            case 'A': rev_kmer[rcount]='T'; break;
            case 'C': rev_kmer[rcount]='G'; break;
            case 'G': rev_kmer[rcount]='C'; break;
            case 'T': rev_kmer[rcount]='A'; break;
            default: rev_kmer[rcount]=ch;
        }
    }
	result=_set_canonical_(aBF,kmer_len,kmer,rev_kmer,k); //h(kmer)<h(rev_kmer)?return 0: return 1	
    // printf("Original %s\n",kmer);
    // printf("Reverse %s\n",rev_kmer);

    while(count<total_kmers){ 
        for(kcount=1,rcount=kmer_len-1;kcount<kmer_len;kcount++,rcount--){
			kmer[kcount-1]=kmer[kcount];
			rev_kmer[rcount]=rev_kmer[rcount-1];
        }
		ch=buff[bindex++]; 
		kmer[--kcount]=ch;		
        switch(ch){
            case 'A': rev_kmer[0]='T'; break;
            case 'C': rev_kmer[0]='G'; break;
            case 'G': rev_kmer[0]='C'; break;
            case 'T': rev_kmer[0]='A'; break;
            default: rev_kmer[0]=ch;
        }		
		result=_set_canonical_(aBF,kmer_len,kmer,rev_kmer,k);
        count++; 
    }
	end=clock();
        // printf("Original %s\n",kmer);
        // printf("Reverse %s\n",rev_kmer);
	close(f); 

	printf("Insertion complete!\n\n");
	printf("Total insertion:%llu\n",count);	
	printf("Elapsed Time of insertion:%f\n\n", (double)(end-start)/CLOCKS_PER_SEC);
	double mb=8*1024*1024.0;
	printf("\nRequired memory size in bits: %lu\n", size);
	printf("\nTotal memory size in MB: %lf\n", (double)(size)/mb);
	printf("\nRequired memory size in bits: %lu\n", m);
	printf("\nRequired memory size in MB: %lf\n", (double)(m)/mb);

	// ################### Writing result #############
	fres=fopen(fname[4],"a+"); //Writing to Result File 
	if(fres==NULL) { 
		printf("Result File can't be created/Opened\n");
		exit(0);
	}
	fprintf(fres,"\n\n########### Results of dataset %s without write (Canonical) ############\n",fname[0]);
	fprintf(fres,"Kmer length: %d\n",kmer_len);
	fprintf(fres,"Total kmers: %llu\n",total_kmers);
	fprintf(fres,"Total insertion: %llu\n",count);
	fprintf(fres,"Elapsed Time of insertion: %f\n", (double)(end-start)/CLOCKS_PER_SEC);
	fprintf(fres,"Required memory size in bits and MB: %lu\t %lf\n", size,(double)(size)/mb);
	fprintf(fres,"Required memory size in bits: %lu\t %lf\n", m,(double)(m)/mb);
	fclose(fkmer);

	_free_(aBF);
}	

//#### Insertion with file writing (Canonical) ##########
void insertion_canonical_with_filewrite(char fname[6][100],int kmer_len,int threshold, int k){
	int result,kcount=0,rcount=0;
    char ch,kmer[kmer_len+1],rev_kmer[kmer_len+1];
	double err=0.001;
	unsigned long int m,r;
	double fp=0.0;
	clock_t start, end;
	unsigned long int h1,h2;
	long long int count,fileLength,total_kmers;
	FILE *fkmer,*ferror,*ftrust,*fres;

	printf("Insertion process\n");
    int f = open(fname[0], O_RDONLY);
    if (f ==-1)
        printf("Error in opening file\n");  

	char cmd[150]="wc -c <", *s1=" > linecount.txt"; //Do not change the value of the string
	strcat(cmd,fname[0]);
	strcat(cmd,s1);
    system(cmd); // calculate #characters and save in result.txt
    
	fkmer=fopen("linecount.txt","r");
	if(fkmer==NULL) {
		printf("File can't be created\n");
		exit(0);
	}
	fscanf(fkmer,"%lld",&fileLength); 
    
    printf("File length: %lld\n",(long long)fileLength); 
	total_kmers=fileLength-kmer_len; //Total number of kmers in the file
    printf("No of kmers: %lld\n",(long long)total_kmers); //print
	fclose(fkmer);

	printf("File initiated!\n");	// Filter construction
	
	//CONSTRUCT RBF
	
	int mem=(int)(total_kmers);
	m=memory(mem,err);
	printf("Memory initiated!\n");
	setDim(m);
	printf("Dimensions initiated!\n");
	aBF=allocate(); printf("Filters are created!\n");
    distinct_robustBF=allocate(); // Distinct RBF created
	printf("Distinct Robust Filters are created!\n");
	trustworthy_robustBF=allocate(); //Trustworthy RBF created
	printf("Trustworthy Robust Filters are created!\n");
	erroneous_robustBF=allocate();//ErroneousRBF created
	printf("Erroneous Robust Filters are created!\n");

    char* buff = mmap(NULL, fileLength, PROT_READ, MAP_SHARED, f, 0); //Map its contents into memory.
	fkmer=fopen(fname[1],"w"); //Writing to Kmer File (distinct kmers)
	ftrust=fopen(fname[3],"w");
	if(fkmer==NULL) { 
		printf("Distinct Kmer File can't be created\n");
		exit(0);
	}
    off_t bindex=0;	
	kmer[kmer_len]='\0';
    rev_kmer[kmer_len]='\0';
	start=clock(); //Starting clock
    for(kcount=0,rcount=kmer_len-1;kcount<kmer_len;kcount++,rcount--){ //Inserting first kmer
        ch=buff[bindex++]; //printf("ch: %c\n",ch);
		kmer[kcount]=ch;
        switch(ch){
            case 'A': rev_kmer[rcount]='T'; break;
            case 'C': rev_kmer[rcount]='G'; break;
            case 'G': rev_kmer[rcount]='C'; break;
            case 'T': rev_kmer[rcount]='A'; break;
            default: rev_kmer[rcount]=ch;
        }
    }
	result=_set_canonical_(aBF,kmer_len,kmer,rev_kmer,k); //h(kmer)<h(rev_kmer)?return 0: return 1
    if (result==0)   
		fprintf(fkmer,"%s\n",kmer); 
    else
        fprintf(fkmer,"%s\n",rev_kmer);   

	count=1; 
    // printf("Original %s\n",kmer); 
    // printf("Reverse %s\n\n",rev_kmer);
	
    while(count<total_kmers){ \
        for(kcount=1,rcount=kmer_len-1;kcount<kmer_len;kcount++,rcount--){
                kmer[kcount-1]=kmer[kcount];
                rev_kmer[rcount]=rev_kmer[rcount-1];
        }
        ch=buff[bindex++];
		kmer[--kcount]=ch; 
        switch(ch){
            case 'A': rev_kmer[0]='T'; break;
            case 'C': rev_kmer[0]='G'; break;
            case 'G': rev_kmer[0]='C'; break;
            case 'T': rev_kmer[0]='A'; break;
            default: rev_kmer[0]=ch;
        }
		if(_test_canonical_(aBF,kmer_len,kmer,rev_kmer,k,&result)==0){ //result:-h(kmer)<h(rev_kmer)?return 0: return 1			
			if(result==0){	 
				if(lookup_keyBF(distinct_robustBF,kmer,kmer_len)==0){
					fprintf(fkmer,"%s\n",kmer); //insert() of robustbf for distinct
					insert_keyBF(distinct_robustBF,kmer,kmer_len);
				}
				else{
					if(lookup_keyBF(trustworthy_robustBF,kmer,kmer_len)==0){
						fprintf(ftrust,"%s\n",kmer); //insert() of robustbf for trustworthy
						insert_keyBF(trustworthy_robustBF,kmer,kmer_len);
					}
				}
				_set_(aBF,kmer_len,kmer,k);
			}
			else{
				if(lookup_keyBF(distinct_robustBF,rev_kmer,kmer_len)==0){
					fprintf(fkmer,"%s\n",rev_kmer); //insert() of robustbf for distinct
					insert_keyBF(distinct_robustBF,rev_kmer,kmer_len);
				}
				else{
					if(lookup_keyBF(trustworthy_robustBF,rev_kmer,kmer_len)==0){
						fprintf(fkmer,"%s\n",rev_kmer); //insert() of robustbf for trustworthy
						insert_keyBF(trustworthy_robustBF,rev_kmer,kmer_len);
					}
				}
				_set_(aBF,kmer_len,rev_kmer,k);
			}
		}
		else{
			if(result==0)		     
				_set_(aBF,kmer_len,kmer,k);
			else
				_set_(aBF,kmer_len,rev_kmer,k);
		}
        count++;      
    }
	end=clock();
	// printf("Original %s\n",kmer);
    // printf("Reverse %s\n\n",rev_kmer);
	
	close(f); 
	fclose(fkmer);

	printf("Insertion complete!\n\n");
	printf("Total insertion:%llu\n",count);	
	printf("Elapsed Time of insertion:%f\n\n", (double)(end-start)/CLOCKS_PER_SEC);
	TP=0;TN=0;FP=0;
	double mb=8*1024*1024.0;
	//unsigned long int s=size;
	printf("\nRequired memory size in bits: %lu\n", size);
	printf("\nTotal memory size in MB: %lf\n", (double)(size)/mb);
	printf("\nRequired memory size in bits: %lu\n", m);
	printf("\nRequired memory size in MB: %lf\n", (double)(m)/mb);

	fres=fopen(fname[4],"a+"); //Writing to Result File 
	if(fres==NULL) { 
		printf("Result File can't be created/Opened\n");
		exit(0);
	}
	fprintf(fres,"\n\n########### Results of dataset %s with write (Canonical) ############\n",fname[0]);
	fprintf(fres,"Kmer length: %d\n",kmer_len);
	fprintf(fres,"Total kmers: %llu\n",total_kmers);
	fprintf(fres,"Total insertion: %llu\n",count);
	fprintf(fres,"Elapsed Time of insertion: %f\n", (double)(end-start)/CLOCKS_PER_SEC);
	fprintf(fres,"Required memory size in bits and MB: %lu\t %lf\n", size,(double)(size)/mb);
	fprintf(fres,"Required memory size in bits and MB: %lu\t %lf\n", m,(double)(m)/mb);
	

	//########################## Classification ##########################################    
	fkmer=fopen(fname[1],"r");
	ferror=fopen(fname[2],"w");
	if(fkmer==NULL && ferror==NULL && ftrust==NULL) { 
		printf("Distinct file or erroneous kmers file or trustworthy kmer file can't be created\n");
		exit(0);
	}
	
	fscanf(fkmer,"%s",kmer);
	count=1;
	start=clock(); //Starting clock
	while(!feof(fkmer)){	
		if(lookup_keyBF(trustworthy_robustBF,kmer,kmer_len)==0){
			fprintf(ferror,"%s\n",kmer); //insert() of robustbf for erroneous
			insert_keyBF(erroneous_robustBF,kmer,kmer_len);
		}
		fscanf(fkmer,"%s",kmer);
		count++;
	}
	end=clock(); //Ending clock
	fclose(fkmer);
	fclose(ftrust);
	fclose(ferror);

	// ################### Writing result #############
	printf("Elapsed Time of classification: %f\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Total queries: %llu\n",count-1);
	fprintf(fres,"\nElapsed Time of query: %f\n", (double)(end-start)/CLOCKS_PER_SEC);
	fprintf(fres,"Total queries: %llu\n",count);
	char *s="wc -l < ";
	for(int i=1;i<4;i++){
		strcpy(cmd,s); //Do not change the value of the string		
		strcat(cmd,fname[i]);
		strcat(cmd,s1); //*s1=" > linecount.txt"
		//printf("Filename: %s\n",cmd);
		system(cmd);

		ferror=fopen("linecount.txt","r");
		if(ferror==NULL) {
			printf("File can't be created\n");
			exit(0);
		}
		fscanf(ferror,"%lld",&fileLength); 
		fclose(ferror);
		if(i==1) //fileLength+1: system call calculates one less number of lines
			fprintf(fres,"Total number of distinct kmers: %lld\n",(long long)fileLength+1); 
		else if(i==2)
			fprintf(fres,"Total number of trustworthy kmers: %lld\n",(long long)fileLength+1); 
		else
			fprintf(fres,"Total number of erroneous kmers: %lld\n",(long long)fileLength+1); 
	}
	fclose(fres);
	_free1_(aBF,distinct_robustBF,trustworthy_robustBF,erroneous_robustBF);
}

int main(int argc, char* argv[])
{
	//Parameters 
    int kmer_len, threshold, k; //kmer_len: Length of the kmer, k:No. of hash functions
    char fname[5][100]={"0","Distinct.txt","Errorneous.txt","Trustworthy.txt","Result.txt"};
    char* parameter[]={"-K","-eta","-h"};

    if(argc==2){
        printf("Check the command format in README file!\n"); 
        printf("Considering the default values: K=28, threshold=5 and number of hash function: 1\n\n");
        kmer_len=28;
        threshold=5;
        k=1;
        strcpy(*fname,argv[1]); // Assigning first file name to the user provided file name
    }
    else if (argc==8){
        for (int i=1;i<6;i=i+2){ //Assiging user provided vales to parameters
            if (strcmp(argv[i],parameter[0])==0)
                kmer_len=atoi(argv[i+1]);
            else if(strcmp(argv[i],parameter[1])==0)
                threshold=atoi(argv[i+1]);
            else if(strcmp(argv[i],parameter[2])==0)
                k=atoi(argv[i+1]);
            else
            { 
                printf("Check the command format in README file!\n");  exit(0);
            }
        }
        strcpy(*fname,argv[7]); // Assigning first file name to the user provided file name
    }
    else 
    {
        printf("Check the command format in README file!\n"); 
        printf("Atleast provide genomic dataset in fasta format!\n");
        exit(0);
    }

    // printf("kmer_len= %d\n",kmer_len);
    // printf("threshold= %d\n",threshold);
    // printf("k= %d\n",k);

	insertion_canonical_without_filewrite(fname,kmer_len,threshold,k); //Canonical
	insertion_canonical_with_filewrite(fname,kmer_len,threshold,k); //Canonical

	return 0;
}
