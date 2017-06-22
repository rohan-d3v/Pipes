/*
Rohan Krishna Ramkhumar
Case ID: rxr353
Assignment 3: Pipes
*/

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define coop 1		//Child option to cooperate
#define def 0		//Child Option to defect
#define terminate '\0'//Parent exit request

char inb[1024];	//Input Buffer
int inbp = 0;	//Input Buffer Pointer
char outb[1024];//Output Buffer
int outbp = 0;	//Output Buffer Pointer

//All Used methods
int process();
int randc();
int puppet(int tpipe[2], int fpipe[2], int pid);
void choice (int opt, int child);

/*
Main method to print out the games
*/
int main(void){

	//Basic Info Print
	printf("Rohan Krishna Ramkhumar \n");
	printf("Case ID: rxr353 \n");
	printf("Assignment 3: Pipes \n\n");
	fflush(stdout);

	process(10);//calls the whole game method, no. of games can be changed as needed

	return 0;
}

//Main Process
int process(int games){

	//Four pipes (read and write)
	int c1pipe1 [2];//to
	int c1pipe2 [2];//from
	int c2pipe1 [2];//to
	int c2pipe2 [2];//from

	pipe(c1pipe1); //The to Pipe for prisoner 1 
	pipe(c2pipe1); //The from Pipe for prisoner 1
	pipe(c1pipe2); //The to Pipe for prisoner 2
	pipe(c2pipe2); //The from Pipe for prisoner 2

	printf("Parent PID: %d\n", getpid());//Prints out parent pid for ref
	fflush(stdout);


	//Child 1 Fork
	pid_t c1 = fork();
	//Prisoner[Child] 1 method
	if (c1 == 0){
		printf("Child [Prisoner] 1 PID: %d\n", getpid());//Child 1 PID
		fflush(stdout);

		srand(getpid());//Seeds rand num
		close(c1pipe1[1]);//Closes pipe 1 write
		close(c1pipe2[0]);//Closes pipe 1 read

		//Loop to print out options until exit (3) is called
		while (1){
			puppet(c1pipe1, c1pipe2, getpid());	//calls the puppet method
		}
	}

	//Child 2 Fork
	pid_t c2 = fork();
	//Prisoner[Childs] 2 method
	if (c2 == 0){
		printf("Child [Prisoner] 2 PID: %d\n\n", getpid());//Child 2 PID
		fflush(stdout);

		srand(getpid());//Seeds rand num
		close(c2pipe1[1]);//Closes pipe 2 write
		close(c2pipe2[0]);//Closes pipe 2 read

		while (1){//loops until parent wants it to exit
			puppet(c2pipe1, c2pipe2, getpid());//Calls the puppet method
		}
	}

	close(c1pipe1[0]); //CLoses Read/to end of pipe 1 [c1]
	close(c2pipe1[0]); //CLoses Write/from end of pipe 1 [c1]
	close(c1pipe2[1]); //Closes Read/to end of pipe 2 [c2]
	close(c2pipe2[1]); //Closes Write/from end of pipe 2 [c2]

	/*
	Output Buffer prep
	*/
	outb[0] = 1;
	outb[1] = terminate;
	outbp = 2;

	write(c1pipe1[1], outb, outbp);//c1 write
	write(c2pipe1[1], outb, outbp);//c2 write
	
	//Child 1 Read
	inbp = 0;
	inb[0] = '*';//Non "terminate" character

	while ((read(c1pipe2[0], &inb[inbp], 1)) && (inbp < 1024) && (inb[inbp] != terminate))
		inbp++;

	sscanf(inb,"%d", &c1);

	//Child 2 Read
	inbp = 0;
	inb[0] = '*';//Non "terminate" character

	while ((read(c2pipe2[0], &inb[inbp], 1)) && (inbp < 1024) && (inb[inbp] != terminate))
		inbp++;

	sscanf(inb,"%d", &c2);


	int c1choice = 0;//c1 coop or defect
	int c2choice = 0;//c2 coop or defect

	double c1points = 0.0;//c1 number of years
	double c2points = 0.0;//c2 number of years

	outb[0] = 2;//Parent asks for choice
	outb[1] = terminate;
	outbp = 2;

	int i = 1;//number of games loop
	while(i <= games){//game loop
		printf("Game %d\n", i);

		write(c1pipe1[1], outb, 2);//c1 write
		read(c1pipe2[0], &inb, 2); //c1 read
		sscanf(inb, "%d", &c1choice);//gets choice from c1

		choice(c1choice, c1); //uses choice method to get the corresponding choice

		write(c2pipe1[1], outb, 2);//c2 write
		read(c2pipe2[0], inb, 2);  //c2 read
		sscanf(inb, "%d", &c2choice);//gets choice from c2

		choice(c2choice, c2); //uses choice method to get the corresponding choice

		if((c1choice == coop) && (c2choice == coop)){//Both cooperatee
			c1points +=  0.5; //+ 6 months
			c2points +=  0.5; //+ 6 months
		}
		if((c1choice == coop) && (c2choice == def)){//Prisoner 1 coop & prisoner 2 def
			c1points +=  10; //+ 10 years
			c2points +=  0;  //No years
		}
		if((c1choice == def) && (c2choice == coop)){//Prisoner 1 def & prisoner 2 coop
			c1points +=  0; //no years
			c2points +=  10;//+ 10 years
		}
		if((c1choice == def) && (c2choice == def)){//Prisoner 1 def & prisoner 2 def
			c1points +=  6; //+ 6 years
			c2points +=  6; //+ 6 years
		}
		i++;//iterator++ in loop
	}

	outb[0] = 3;//child exit (3)
	outb[1] = terminate; //Terminate '/0' 
	outbp = 2;
	write(c1pipe1[1], outb, outbp);
	write(c2pipe1[1], outb, outbp);

	//Pipe closures
	close(c1pipe1[1]);
	close(c2pipe1[1]);
	close(c1pipe2[0]);
	close(c2pipe2[0]);

	//Prints out score
	printf("------------------------\n");
	printf("Score:\n");
	printf("%d [Prisoner 1]: %.1f years\n", c1, c1points);
	printf("%d [Prisoner 2]: %.1f years\n", c2, c2points);
	fflush(stdout);

	return (0);
}

/*
The Child processes for looping until needed
*/
int puppet(int tpipe[2], int fpipe[2], int pid){
	read(tpipe[0], inb, 2);//get the parent request

	if (inb[0]== 1){//If parent requests PID
				outbp =sprintf(outb, "%d", pid);
				outbp++;//Changes pointer to final "terminate" location
				outb[outbp] = terminate;
				write(fpipe[1], outb, outbp);
			}

			if (inb[0]== 2){//Parent requests Prisoner's dilemma choices
				sprintf(outb, "%d", randc());
				outb[1] = terminate;
				write(fpipe[1], outb, 2);
			}

			if (inb[0]== 3){//Parent exit
				close(tpipe[0]);//Closes read/to of pipe
				close(fpipe[1]);//Closes write/ from end of pipe
				_exit(0);
			}
}
/*
Method randc
Generates a Random Number
Uses modulus to then get 0 or 1
0 = Defect
1 = Cooperate
*/
int randc(void){
	return rand()%2;
}
/*
Method choice
Prints out your choice
0 = Defect
else
cooperate
*/
void choice(int opt, int child){
	if (!opt)
		printf("%d: Defect\n", child);
	else
		printf("%d: Cooperate\n", child);
}
