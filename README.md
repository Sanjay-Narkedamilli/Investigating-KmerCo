# KmerCo: A lightweight K-mer counting technique with a tiny memory footprint

## Abstract
----------------
K-mer counting is a requisite process for DNA assembly because it speeds up the overall process. The frequency of K-mers is used for estimating the parameters of DNA assembly, error correction, etc. The process also provides a list of distinct K-mers, which assist in searching large databases and reducing the size of de Bruijn graphs. Nonetheless, K-mer counting is a data and compute-intensive process. Hence, it is crucial to implement a lightweight data structure that occupies low memory but performs fast processing of K-mers. A lightweight K-mer counting technique, called KmerCo, implements a potent counting Bloom Filter variant, called countBF. KmerCo has two phases: insertion and classification. The insertion phase inserts all K-mers into countBF and determines distinct K-mers. The classification phase is responsible for classifying distinct K-mers into trustworthy and erroneous K-mers based on a user-provided threshold value. We have investigated the performance of KmerCo by replacing the default countBF technique with two robustBFs for the insertion and classification phases, similar to the Kcoss Bloom filter. We conducted rigorous experiments to demonstrate the superiority of the modified KmerCo compared to the original KmerCo proposed by Ripon. The experiments were conducted using DNA sequences from four organisms. The datasets were pruned to generate four datasets of different sizes. The modified KmerCo exhibited a positive trustworthy rate compared to the original KmerCo.

## Programming Language: C

### Executing and Running Code
----------------------------

#### Preparing the input file

Retrieve sequences into a text file

```$ awk '{if(NR%4==2)print $0}' dataset.fastq > sequence.txt```

Concatenating all DNA sequences into a single line

```$ sed -i ':a; N; s/\\n/ /; ta' sequence.txt```

#### Compiling

```$ gcc KmerCo.c -o KmerCo -lm```

#### Executing

```$ ./KmerCo -K 28 -eta 5 -h 1 sequence.txt```


SYNOPSIS

        ./KmerCo [-K <K-mer length>] [-eta <threshold>] [-h <#hash()>] -o <out-file> <files>...

OPTIONS

        <K-mer length>    length of K-mers (default = 28)
        <threshold>    Classification of distinct K-mers between trustworthy and erroneous based on this value (default = 5)
			Distinct K-mers: 
        <#hash()> Number of hash functions for the countBF



Note:
1. Trustworthy K-mer: Distinct K-mer having frequncy more than threshold value.
Erroneous K-mer: Disticnt K-mer having frequncy less than or euqal to threshold value.
2. Default countBF counter length=8
3. If changing the countBF counter length include the corresponding mask header file.
4. KmerCo considers canonical K-mers (Refer the paper for defination)

# Citations
-------------

countBF:

```Sabuzima Nayak and Ripon Patgiri. 2021. countBF: A general-purpose high accuracy and space efficient counting bloom filter. In 2021 17th International Conference on Network and Service Management (CNSM). IEEE, 355–359.``` 


# Authors
-------------
1. Sanjay Narkedamilli
Mail: narkedamilli20_ug@cse.nits.ac.in

2. Yogesh Kalluri
Mail: 

