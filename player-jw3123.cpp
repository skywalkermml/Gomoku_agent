/*Name Joshua Wang, UNI: jw3123
The source code for gomoku agent with alpha beta searching
Date:started 11/12 2013
*/
#include <iostream>
#include <stdlib.h>
#include <stdlib.h>     
#include <time.h>   
#include <vector>    
using namespace std;

int timelimit; // timelimit each step
int sttime;  // start time of a step
bool timeup;
const int INF=247483647; // 1/10 of the maximum of int

class Board  // a status of searching and moving forward 
{
public:
char ** board;  // the board recording current status
int n; // size of board
int m;  // winning goal     
int status; // -1: x wins, 0:ongoing, 1: o win 2 :draw
bool isxsmove;
int numonboard; // uesd to check is board is full
int *battlefield;  //the range of the game 0 leftmost 1 rightmost 2 upmost 3 downmost
int * lastmove;
Board(int, int); // constructor of the new board 
Board(Board *, int, int, bool);  // constructor of the new board after a move
~Board(); 
void show();
Board * place(int ,int); // placing a stone 
};

Board::Board(int in_n, int in_m) // constructor of the new board
{
//initialization
n=in_n;
m=in_m;
numonboard=0;
isxsmove=0;  // 0th move is o move, so that 1th will be x th
status=0;
lastmove= new int [2];
lastmove[0]= n/2;
lastmove[1]= n/2;
board= new char * [n];
for (int i=0; i<=n-1; i++)
	board[i]= new char[n]; 
    
// creating board
for (int i=0; i<=n-1; i++)
    { for (int j=0; j<=n-1; j++)
       board[i][j]= '.'; 
    }

battlefield = new int [4];
battlefield[0]=max(0, (n/2)-2);
battlefield[1]=min(n-1, (n/2)+2);
battlefield[2]=max(0, (n/2)-2);
battlefield[3]=min(n-1, (n/2)+2);
}

Board::Board(Board *inputboard, int row, int col, bool next_isxsmove)
{
/*inputboard = last status; put stone on row, col on x's move if (next_isxsmove)  */
n=inputboard->n; 
m=inputboard->m;

numonboard=inputboard->numonboard+1;   // this is costomed for generating new move
isxsmove=!inputboard->isxsmove;   // this is costomed for generting new move

battlefield = new int [4];
battlefield[0]=inputboard->battlefield[0];
battlefield[1]=inputboard->battlefield[1];
battlefield[2]=inputboard->battlefield[2];
battlefield[3]=inputboard->battlefield[3];

lastmove= new int [2];
lastmove[0]= row;
lastmove[1]= col;

board= new char * [n];
for (int i=0; i<=n-1; ++i)
	board[i]= new char[n]; 
    

for (int i=0; i<=n-1; ++i)   // hard copy 
    { for (int j=0; j<=n-1; ++j)
      board[i][j]= inputboard->board[i][j]; 
    }

board[row][col]= (isxsmove)? 'x':'o'; // laid stone

//battlefield extended::
if (col-2<battlefield[0]){battlefield[0]=max(col-2,0);}
else if (col+2>battlefield[1]) {battlefield[1]=min(col+2,n-1);}

if (row-2<battlefield[2]){battlefield[2]=max(row-2,0);}
else if (row+2>battlefield[3]) {battlefield[3]=min(row+2,n-1);}

// checking status
char oxtern=board[row][col];
int maxlength[4]={0,0,0,0}; //check status 0: horizon, 1:vertical, 2: NW to SE 3: NE to SW

status = 0; // assume ongoing  

for (int i=battlefield[2]; i<=battlefield[3]; ++i)   
    { for (int j=battlefield[0]; j<=battlefield[1]; ++j)
      {
		    if (i==row)  // on the row of the new stone 
			    {if (board[i][j]==oxtern) 
				     {maxlength[0]+=1;
				        if (maxlength[0]==m)  // oxtern wins
				  	       {status = (isxsmove)? -1: 1; 
                   break;}
                /*else if (maxlength[0]>m)
                        {cout<<"you got >m stones, but according to the rule, only exactly m stone is wining."<<endl;}  */
				      } // end if (board[i][j]==oxtern) 
			     else maxlength[0]=0;
			    }  // end if (i==row)

		    if (j==col) // on the col of the new stone 
			    {if (board[i][j]==oxtern) 
				     {maxlength[1]+=1;
				      if (maxlength[1]==m) 
                 {status = (isxsmove)? -1: 1;
				  	     break;} 
             /* else if (maxlength[1]>m)
                      {cout<<"you got >m stones, but according to the rule, only exactly m stone is wining."<<endl;}*/
				     }
			    else maxlength[1]=0 ; 
			    }

		    if (i-j==row-col) // southwest to northwest  
			    {if (board[i][j]==oxtern) 
				     {maxlength[2]+=1;
				      if (maxlength[2]==m) 
                  {status = (isxsmove)? -1: 1;
				  	      break;} 
				     /* else if (maxlength[2]>m)
                      {cout<<"you got >m stones, but according to the rule, only exactly m stone is wining."<<endl;}*/
             }
			     else maxlength[2]=0  ;
			    }

		    if (i+j==row+col) // southeast to northwest 
			     {if (board[i][j]==oxtern) 
				      {maxlength[3]+=1;
				       if (maxlength[3]==m) 
				          {status = (isxsmove)? -1: 1;
				  	      break;} 
              /* else if (maxlength[3]>m)
                      {cout<<"you got >m stones, but according to the rule, only exactly m stone is wining."<<endl;}	*/			
              }
			      else maxlength[3]=0 ; 
			     }
      } // end of inner for loop 
    }// end for checking status
 

if (status == 0) // if status does not change 
  {status= (numonboard==n*n)? 2:0 ;}
}  // end consturctor 

Board::~Board()
{

for (int i=0; i<=n-1; ++i)
	delete [] board[i] ; 
delete [] board;
delete [] battlefield;
delete [] lastmove;
}



void Board::show() // print the status
{

for (int i=0; i<=n-1; i++)
   {for (int j=0; j<=n-1; j++)
     {cout<<board[i][j]<<" ";} 
     cout<<endl;}
   
}



Board *Board::place(int row, int col) // the action of a player
{
if ( row>n-1 || row<0 || col>n-1|| col<0 ||board[row][col]!='.')
	{cout<<"error, cannot put at that position"<<endl;
     return this;
	}
else // legal move
	{
	Board * nextboard = new Board (this, row, col, isxsmove);
	return nextboard;
	} 

}



class ABsearch  // the class that realize minmax search with alpha beta pruning 
{
public:
int * nextstep(Board*, bool);
int maxvalue (Board*, int alpha, int Beta ,int level, int levellimit); 
int minvalue (Board*,int alpha, int Beta,int level, int levellimit);
bool isx;
private:
int *nextstep_sol;
int eval_state(Board*);
};



int * ABsearch::nextstep(Board * inputboard, bool inisx)
{
nextstep_sol= new int [2];
nextstep_sol[0]=-1;
nextstep_sol[1]=-1;
isx=inisx;

int score=maxvalue(inputboard, -2*INF, 2*INF, 1, 2);
int bestrow=nextstep_sol[0];
int bestcol=nextstep_sol[1];
int levellimit=3;
timeup=0; // not timeup
while(!timeup)
{
if (maxvalue(inputboard, -2*INF, 2*INF, 1, levellimit)> score)
{
  bestrow=nextstep_sol[0];
  bestcol=nextstep_sol[1];
}
  levellimit++;
}

nextstep_sol[0]=bestrow;
nextstep_sol[1]=bestcol;


// remedy to prevent crash;
if (nextstep_sol[0]==-1||nextstep_sol[1]==-1)
{for (int i=0; i<=inputboard->n-1;i++)
  for (int j=0; j<=inputboard->n-1;j++)
    {if (inputboard->board[i][j]=='.')
      {
       nextstep_sol[0]=i;
       nextstep_sol[1]=j;
       break; 
      }
    }
}
return nextstep_sol;
}

int ABsearch::maxvalue(Board * inputboard, int alpha, int  beta, int level, int levellimit)
{
// termianl test
int score=eval_state(inputboard);
timeup=timelimit-(time(NULL)-sttime)<2^(level/4);
if (score==-INF || score==INF||level==levellimit|| timeup)
  {if (level !=1 ) 
    {delete inputboard;}
   return score;
}


int value=-INF-1;
int newvalue=-INF-2;
int * center =inputboard->lastmove;
int i,j;
bool checked [inputboard->n][inputboard->n];
for (int i =0; i<=inputboard->n-1;i++)
 {for (int j=0; j<= inputboard->n-1; j++)
 {
 if (j>=inputboard->battlefield[0]&&
     j<=inputboard->battlefield[1]&&
     i>=inputboard->battlefield[2]&&
     i<=inputboard->battlefield[3])
  {checked[i][j]=0;}
  else {checked[i][j]=1;}
 }} 


for(int radius=1; center[1]-radius>=inputboard->battlefield[0]||
                  center[1]+radius<=inputboard->battlefield[1]||
                  center[0]-radius>=inputboard->battlefield[2]||
                  center[0]+radius<=inputboard->battlefield[3];++radius){
	for (int offset=-radius; offset<=radius ; offset++) 
         { for (int dir=1; dir<=4;dir++)
           { 

            switch(dir) { 
        case 1: //up
            i=center[0]-radius;
            j=center[1]+offset; 
            break; 
        case 2 : //down: 
            i=center[0]+radius;
            j=center[1]+offset;  
            break; 
        case 3:  //left
            i=center[0]+offset;
            j=center[1]-radius;   
            break; 
        case 4: // right
            i=center[0]+offset;
            j=center[1]+radius; 
            break; 
        default: 
            cout << "program should not go here" <<endl; 
    }  
         
         if (i<0 || i>inputboard->n-1 || j<0 || j>inputboard->n-1) {continue;}
  
         if (checked[i][j]==0) // not checked
         {
          checked[i][j]=1;
         } 
         else
         {continue;}

         if(inputboard->board[i][j]!='.') {continue;}
        
         newvalue=minvalue(inputboard->place(i,j),alpha, beta, level+1, levellimit);

         if (newvalue>value)
           {value= newvalue;
           		 
              if (level==1) 
                {
                nextstep_sol[0]=i;
                nextstep_sol[1]=j;
                }


             if (value> beta&& level != 1) // pruning, this node no longer needed 
           		{ 
           		 delete inputboard;
               return value;
           		}

           	 alpha= max(alpha, value);
           }
         }}} // end for loops

if (level !=1 ) {delete inputboard;}
return value;
}


int ABsearch::minvalue(Board * inputboard, int alpha, int  beta, int level, int levellimit)
{
// termianl test
int score=eval_state(inputboard);
timeup=(timelimit-(time(NULL)-sttime)<2^(level/4));
if (score==-INF || score==INF||level==levellimit|| timeup)
{delete inputboard;
return score+1-level;}


int value=INF+1;
int newvalue=INF+2;
int * center =inputboard->lastmove;
int i,j;
bool checked [inputboard->n][inputboard->n];
for (int i =0; i<=inputboard->n-1;i++)
 {for (int j=0; j<= inputboard->n-1; j++)
 {
 if (j>=inputboard->battlefield[0]&&
     j<=inputboard->battlefield[1]&&
     i>=inputboard->battlefield[2]&&
     i<=inputboard->battlefield[3])
  {checked[i][j]=0;}
  else {checked[i][j]=1;}
 }} 


for(int radius=1; center[1]-radius>=inputboard->battlefield[0]||
                  center[1]+radius<=inputboard->battlefield[1]||
                  center[0]-radius>=inputboard->battlefield[2]||
                  center[0]+radius<=inputboard->battlefield[3];++radius){
  for (int offset=-radius; offset<=radius ; offset++) 
         { for (int dir=1; dir<=4;dir++)
           { 

            switch(dir) { 
        case 1: //up
            i=center[0]-radius;
            j=center[1]+offset; 
            break; 
        case 2 : //down: 
            i=center[0]+radius;
            j=center[1]+offset;  
            break; 
        case 3:  //left
            i=center[0]+offset;
            j=center[1]-radius;   
            break; 
        case 4: // right
            i=center[0]+offset;
            j=center[1]+radius; 
            break; 
        default: 
            cout << "program should not go here" <<endl; 
    }  
         
         if (i<0 || i>inputboard->n-1 || j<0 || j>inputboard->n-1) {continue;}
  
         if (checked[i][j]==0) // not checked
         {
          checked[i][j]=1;
         } 
         else
         {continue;}

         if(inputboard->board[i][j]!='.') {continue;}
         //cout<<"enter maxvalue"<<endl;
         newvalue=maxvalue(inputboard->place(i,j), alpha, beta, level+1, levellimit);
         if (newvalue<value)
           {value= newvalue;
           		if (value<alpha) // pruning 
           		{ 
               delete inputboard;
               return value;
           		}

           	 beta= min(beta, value);
           }
         }}}

delete inputboard;
return value;
}

int ABsearch:: eval_state(Board * inputboard) //agent isx ; now is xturn
{

if (inputboard->status==-1)
  {  
     if (!isx) return  -INF;
      else return INF;
  }
else if (inputboard->status==1)
  {
     if (isx) return  -INF;
      else return INF;
  }
else if (inputboard->status==2)  //draw
return 0 ;
else  // other possible configuration sets:
{
int n=inputboard->n;
int m=inputboard->m;  
char **board= inputboard->board;
char aichar= (isx)? 'x':'o';
char richar= (isx)? 'o':'x';
int numofopenai[n];  //the agent 
int numofopenri[n];  //the rival
int numofhalfai[n];  
int numofhalfri[n];
for (int i=0; i<=n-1;i++) // initialization
{numofopenai[i]=0;   
numofopenri[i]=0;  
numofhalfai[i]=0;  
numofhalfri[i]=0;}  


// horizon search
for(int i=0; i<=n-1;i++)
{
int accfreeai=0;
int accfreeri=0;
int accbloai=0;
int accblori=0;
char last='N';
for (int j=0; j<=n-1;j++)
  {
   if (last!=board[i][j]) // char change
    { 
     if (last=='N')  // case 1 (about last)
     { 
       if (board[i][j]==aichar) {accbloai=1;} 
       if (board[i][j]==richar) {accblori=1;} 
     }

      else if (last=='.') // case 2 (about last)
     { 
       if (board[i][j]==aichar) {accfreeai=1;} 
       if (board[i][j]==richar) {accfreeri=1;} 
     }

      else if (last==aichar) // case 3 (about last)
     { 
       if (board[i][j]=='.') //case 3.1 from ai to .
        {
         if (accfreeai!=0&&accbloai==0) //it's a both side free
           {
            numofopenai[accfreeai]++;
            accfreeai=0;
           } 
          else if (accbloai!=0&&accfreeai==0)
           {
           numofhalfai[accbloai]++;
           accbloai=0;
           }
          else {cout<<"error in eval_state, program should not go here 1"<<endl;}

         } 
       else if (board[i][j]==richar)  // case 3.2 chage from ai to ri
        {
          if (accfreeai!=0&&accbloai==0) //ai one side free
           {
            numofhalfai[accfreeai]++;
            accfreeai=0;
            accblori=1;
           } 
          else if (accbloai!=0&&accfreeai==0)  // ai both side block, ignore
           {
           accbloai=0;
           accblori=1;
           }
          else {cout<<"error in eval_state, program should not go here 2"<<endl;}
        } 
     }

      else if (last==richar) // case 4 (about last)
     { 
       if (board[i][j]=='.') // case 4.1 from ri to .
       {
        if (accfreeri!=0&&accblori==0) //ri is  both side free
           {
            numofopenri[accfreeri]++;
            accfreeri=0;
           } 
          else if (accblori!=0&&accfreeri==0)  // only one sided free
           {
           numofhalfri[accblori]++;
           accblori=0;
           }
          else {cout<<"error in eval_state, program should not go here 3"<<endl;}
       } 
       else if (board[i][j]==aichar) // case 4.2 from ri to ai
        {
          if (accfreeri!=0&&accblori==0) //ri one side free
           {
            numofhalfri[accfreeri]++;
            accfreeri=0;
            accbloai=1;
           } 
          else if (accblori!=0&&accfreeri==0)  // ri both side block, ignore
           {
           accblori=0;
           accbloai=1;
           }
          else {cout<<"error in eval_state, program should not go here 4"<<endl;}
        } 
     }



    }
   else // char not change continue accumulating
    {
     if (accfreeai!=0) accfreeai++;
     else if (accfreeri!=0) accfreeri++;
     else if (accbloai!=0) accbloai++;
     else if (accblori!=0) accblori++;
     } 
  
    last=board[i][j];

  }
//handle the end of the line
     if (accfreeai!=0) {numofhalfai[accfreeai]++;}
     if (accfreeri!=0) {numofhalfri[accfreeri]++;}
/*sanity check
int numofnon0=0;
if (accfreeai!=0){numofnon0++;}
if (accfreeri!=0){numofnon0++;}
if (accbloai!=0){numofnon0++;}
if (accblori!=0){numofnon0++;}
if (numofnon0>1) {cout<<"error, sanity check in eval fais"<<endl;}*/
}

// vertical search

for(int j=0; j<=n-1;j++)
{
int accfreeai=0;
int accfreeri=0;
int accbloai=0;
int accblori=0;
char last='N';
for (int i=0; i<=n-1;i++)
  {
   if (last!=board[i][j]) // char change
    { 
     if (last=='N')  // case 1 (about last)
     { 
       if (board[i][j]==aichar) {accbloai=1;} 
       if (board[i][j]==richar) {accblori=1;} 
     }

      else if (last=='.') // case 2 (about last)
     { 
       if (board[i][j]==aichar) {accfreeai=1;} 
       if (board[i][j]==richar) {accfreeri=1;} 
     }

      else if (last==aichar) // case 3 (about last)
     { 
       if (board[i][j]=='.') //case 3.1 from ai to .
        {
         if (accfreeai!=0&&accbloai==0) //it's a both side free
           {
            numofopenai[accfreeai]++;
            accfreeai=0;
           } 
          else if (accbloai!=0&&accfreeai==0)
           {
           numofhalfai[accbloai]++;
           accbloai=0;
           }
          else {cout<<"error in eval_state, program should not go here 1"<<endl;}

         } 
       else if (board[i][j]==richar)  // case 3.2 chage from ai to ri
        {
          if (accfreeai!=0&&accbloai==0) //ai one side free
           {
            numofhalfai[accfreeai]++;
            accfreeai=0;
            accblori=1;
           } 
          else if (accbloai!=0&&accfreeai==0)  // ai both side block, ignore
           {
           accbloai=0;
           accblori=1;
           }
          else {cout<<"error in eval_state, program should not go here 2"<<endl;}
        } 
     }

      else if (last==richar) // case 4 (about last)
     { 
       if (board[i][j]=='.') // case 4.1 from ri to .
       {
        if (accfreeri!=0&&accblori==0) //ri is  both side free
           {
            numofopenri[accfreeri]++;
            accfreeri=0;
           } 
          else if (accblori!=0&&accfreeri==0)  // only one sided free
           {
           numofhalfri[accblori]++;
           accblori=0;
           }
          else {cout<<"error in eval_state, program should not go here 3"<<endl;}
       } 
       else if (board[i][j]==aichar) // case 4.2 from ri to ai
        {
          if (accfreeri!=0&&accblori==0) //ri one side free
           {
            numofhalfri[accfreeri]++;
            accfreeri=0;
            accbloai=1;
           } 
          else if (accblori!=0&&accfreeri==0)  // ri both side block, ignore
           {
           accblori=0;
           accbloai=1;
           }
          else {cout<<"error in eval_state, program should not go here 4"<<endl;}
        } 
     }



    }
   else // char not change continue accumulating
    {
     if (accfreeai!=0) accfreeai++;
     else if (accfreeri!=0) accfreeri++;
     else if (accbloai!=0) accbloai++;
     else if (accblori!=0) accblori++;
     } 
  
    last=board[i][j];

  }
//handle the end of the line
     if (accfreeai!=0) {numofhalfai[accfreeai]++;}
     if (accfreeri!=0) {numofhalfri[accfreeri]++;}
/*sanity check
int numofnon0=0;
if (accfreeai!=0){numofnon0++;}
if (accfreeri!=0){numofnon0++;}
if (accbloai!=0){numofnon0++;}
if (accblori!=0){numofnon0++;}
if (numofnon0>1) {cout<<"error, sanity check in eval fais"<<endl;}*/
}

// NW to SE
for (int portion=0; portion<=1; portion++)  // portion 0: SW, including middle, portion 1: NE, excluding middle 
{
int lengthlimit;
if (portion==0) {lengthlimit=n;}
if (portion==1) {lengthlimit=n-1;}
for(int length=m; length<=lengthlimit;length++)  // represent a line
{
int accfreeai=0;
int accfreeri=0;
int accbloai=0;
int accblori=0;
char last='N';
for (int incre=0; incre<=length-1;incre++)
  {
    int i;
    int j;
   if (portion==0)
   {i=n-length+incre;
   j= incre;}
   else if (portion==1)
   {j=n-length+incre;
   i= incre;}


   if (last!=board[i][j]) // char change
    { 
     if (last=='N')  // case 1 (about last)
     { 
       if (board[i][j]==aichar) {accbloai=1;} 
       if (board[i][j]==richar) {accblori=1;} 
     }

      else if (last=='.') // case 2 (about last)
     { 
       if (board[i][j]==aichar) {accfreeai=1;} 
       if (board[i][j]==richar) {accfreeri=1;} 
     }

      else if (last==aichar) // case 3 (about last)
     { 
       if (board[i][j]=='.') //case 3.1 from ai to .
        {
         if (accfreeai!=0&&accbloai==0) //it's a both side free
           {
            numofopenai[accfreeai]++;
            accfreeai=0;
           } 
          else if (accbloai!=0&&accfreeai==0)
           {
           numofhalfai[accbloai]++;
           accbloai=0;
           }
          else {cout<<"error in eval_state, program should not go here 1"<<endl;}

         } 
       else if (board[i][j]==richar)  // case 3.2 chage from ai to ri
        {
          if (accfreeai!=0&&accbloai==0) //ai one side free
           {
            numofhalfai[accfreeai]++;
            accfreeai=0;
            accblori=1;
           } 
          else if (accbloai!=0&&accfreeai==0)  // ai both side block, ignore
           {
           accbloai=0;
           accblori=1;
           }
          else {cout<<"error in eval_state, program should not go here 2"<<endl;}
        } 
     }

      else if (last==richar) // case 4 (about last)
     { 
       if (board[i][j]=='.') // case 4.1 from ri to .
       {
        if (accfreeri!=0&&accblori==0) //ri is  both side free
           {
            numofopenri[accfreeri]++;
            accfreeri=0;
           } 
          else if (accblori!=0&&accfreeri==0)  // only one sided free
           {
           numofhalfri[accblori]++;
           accblori=0;
           }
          else {cout<<"error in eval_state, program should not go here 3"<<endl;}
       } 
       else if (board[i][j]==aichar) // case 4.2 from ri to ai
        {
          if (accfreeri!=0&&accblori==0) //ri one side free
           {
            numofhalfri[accfreeri]++;
            accfreeri=0;
            accbloai=1;
           } 
          else if (accblori!=0&&accfreeri==0)  // ri both side block, ignore
           {
           accblori=0;
           accbloai=1;
           }
          else {cout<<"error in eval_state, program should not go here 4"<<endl;}
        } 
     }



    }
   else // char not change continue accumulating
    {
     if (accfreeai!=0) accfreeai++;
     else if (accfreeri!=0) accfreeri++;
     else if (accbloai!=0) accbloai++;
     else if (accblori!=0) accblori++;
     } 
  
    last=board[i][j];

  }
//handle the end of the line
     if (accfreeai!=0) {numofhalfai[accfreeai]++;}
     if (accfreeri!=0) {numofhalfri[accfreeri]++;}
/* sanity check
int numofnon0=0;
if (accfreeai!=0){numofnon0++;}
if (accfreeri!=0){numofnon0++;}
if (accbloai!=0){numofnon0++;}
if (accblori!=0){numofnon0++;}
if (numofnon0>1) {cout<<"error, sanity check in eval fais"<<endl;}*/
}}

// NE to SW
for (int portion=0; portion<=1; portion++)  // portion 0: SE, including middle, portion 1: NW, excluding middle 
{
int lengthlimit;
if (portion==0) {lengthlimit=n;}
if (portion==1) {lengthlimit=n-1;}
for(int length=m; length<=lengthlimit;length++)  // represent a line
{
int accfreeai=0;
int accfreeri=0;
int accbloai=0;
int accblori=0;
char last='N';
for (int incre=0; incre<=length-1;incre++)
  {
    int i;
    int j;
   if (portion==0)
   {i=n-length+incre;
   j= n-1-incre;}
   else if (portion==1)
   {j=length-1-incre;
   i= incre;}


   if (last!=board[i][j]) // char change
    { 
     if (last=='N')  // case 1 (about last)
     { 
       if (board[i][j]==aichar) {accbloai=1;} 
       if (board[i][j]==richar) {accblori=1;} 
     }

      else if (last=='.') // case 2 (about last)
     { 
       if (board[i][j]==aichar) {accfreeai=1;} 
       if (board[i][j]==richar) {accfreeri=1;} 
     }

      else if (last==aichar) // case 3 (about last)
     { 
       if (board[i][j]=='.') //case 3.1 from ai to .
        {
         if (accfreeai!=0&&accbloai==0) //it's a both side free
           {
            numofopenai[accfreeai]++;
            accfreeai=0;
           } 
          else if (accbloai!=0&&accfreeai==0)
           {
           numofhalfai[accbloai]++;
           accbloai=0;
           }
          else {cout<<"error in eval_state, program should not go here 1"<<endl;}

         } 
       else if (board[i][j]==richar)  // case 3.2 chage from ai to ri
        {
          if (accfreeai!=0&&accbloai==0) //ai one side free
           {
            numofhalfai[accfreeai]++;
            accfreeai=0;
            accblori=1;
           } 
          else if (accbloai!=0&&accfreeai==0)  // ai both side block, ignore
           {
           accbloai=0;
           accblori=1;
           }
          else {cout<<"error in eval_state, program should not go here 2"<<endl;}
        } 
     }

      else if (last==richar) // case 4 (about last)
     { 
       if (board[i][j]=='.') // case 4.1 from ri to .
       {
        if (accfreeri!=0&&accblori==0) //ri is  both side free
           {
            numofopenri[accfreeri]++;
            accfreeri=0;
           } 
          else if (accblori!=0&&accfreeri==0)  // only one sided free
           {
           numofhalfri[accblori]++;
           accblori=0;
           }
          else {cout<<"error in eval_state, program should not go here 3"<<endl;}
       } 
       else if (board[i][j]==aichar) // case 4.2 from ri to ai
        {
          if (accfreeri!=0&&accblori==0) //ri one side free
           {
            numofhalfri[accfreeri]++;
            accfreeri=0;
            accbloai=1;
           } 
          else if (accblori!=0&&accfreeri==0)  // ri both side block, ignore
           {
           accblori=0;
           accbloai=1;
           }
          else {cout<<"error in eval_state, program should not go here 4"<<endl;}
        } 
     }



    }
   else // char not change continue accumulating
    {
     if (accfreeai!=0) accfreeai++;
     else if (accfreeri!=0) accfreeri++;
     else if (accbloai!=0) accbloai++;
     else if (accblori!=0) accblori++;
     } 
  
    last=board[i][j];

  }
//handle the end of the line
     if (accfreeai!=0) {numofhalfai[accfreeai]++;}
     if (accfreeri!=0) {numofhalfri[accfreeri]++;}
/*sanity check
int numofnon0=0;
if (accfreeai!=0){numofnon0++;}
if (accfreeri!=0){numofnon0++;}
if (accbloai!=0){numofnon0++;}
if (accblori!=0){numofnon0++;}
if (numofnon0>1) {cout<<"error, sanity check in eval fais"<<endl;}*/
}}





if (m>=3)
{
if (numofopenai[m]>=1)  return INF;  
if (numofopenri[m]>=1)  return -INF;
if (numofopenri[m-1]>=1)  return -INF/2;
if (numofopenri[m-2]>=2)  return -INF/2;
if (numofhalfri[m-1]>=1&&numofopenri[m-2]>=1)  return -INF/2;
if (numofopenai[m-1]>=1)  return INF/2;
if (numofopenai[m-2]>=2)  return INF/2;
if (numofhalfai[m-1]>=1&&numofopenai[m-2]>=1)  return INF/2;
return 10000*(numofopenai[m-1]-numofopenri[m-1])+1000*(numofhalfai[m-1]-numofhalfri[m-1])\
+500*(numofopenai[m-2]-numofopenri[m-2])+100*(numofhalfai[m-2]-numofhalfri[m-2]);}

else 
return 10000*(numofopenai[m-1]-numofopenri[m-1]);

}  // end else (general function) 

} // end eval function 



int main(int argc, char *argv[])
{

if (argc!=4)
{
cout<<"Error input. Usage: filename n m s, where  \
n is the game board dimension, m is winning chain length, and s is move selection time limit"<<endl; 
exit (1);
}

int n=atoi(argv[1]);
int m=atoi(argv[2]) ;
timelimit=atoi(argv[3]);
int options=7;

while (options != 0)
{
cout<<"size: "<<n<<" wining line: "<<m<<" timelimit: "<<timelimit<<endl<<endl;

cout<<"Please enter the playing mode:"<<endl;
cout<<"1: AI plays with stdin "<<endl;
cout<<"2: AI plays with random player"<<endl;
cout<<"3: AI plays against herself"<<endl;
cout<<"(0: exit)"<<endl;
cin>> options;

 if (options==1)
    {cout<<"Enter an odd number for AI first; even number for stdin first:"<<endl;}
 else if (options==2)
    {cout<<"Enter an odd number for AI first; even number for random player first:"<<endl;}
 int aifirst;
 if (options ==1 ||options ==2)
 {cin>>aifirst;}
 else if (options==3) {aifirst=1;} 
 else if (options==0) {break;} 
Board * thisboard = new Board (n,m);
Board *tempboard;
ABsearch AI;
int * AIsol ;
int status=0;


if (aifirst%2==0) // even, stdin or random first
{



do
{

cout<<"player input position:"<<endl;
int row, col;

thisboard->show();
cout<<endl;
if (options==1) {cin>>row>>col;}

else if (options==2)
{
vector< vector<int> > emptypoints;

for (int i=0; i<=n-1;i++)
  for (int j=0;j<=n-1;j++)
    {if (thisboard->board[i][j]=='.')
      {
       vector<int> temp;
       temp.push_back(i);
       temp.push_back(j);
       emptypoints.push_back(temp);
      }
    }
   int randomindex=rand()%emptypoints.size();
  row=  emptypoints[randomindex][0];
  col=  emptypoints[randomindex][1];
}

tempboard=thisboard->place(row, col);
} while(tempboard==thisboard);

delete thisboard;
thisboard= tempboard;
cout<<endl;
thisboard->show();
}  // end do while



while (thisboard->status==0)
{
sttime=time(NULL);
AIsol= AI.nextstep(thisboard, aifirst%2==1) ; // aifirst%2==1 is ture when AI is x  
cout<< "AI place: "<<AIsol[0]<<" "<<AIsol[1]<<endl;
cout<<"In "<<time(NULL)-sttime<<" seconds"<<endl<<endl; 
tempboard= thisboard->place(AIsol[0], AIsol[1]);
if (tempboard==thisboard) 
  {cout<<"error, AI put at a wrong place"<<endl;}
else {
delete thisboard;
thisboard= tempboard;
delete AIsol;
}


thisboard->show();
cout<<endl;

if (thisboard->status!=0) break;

do{
int row, col;

if (options==1) 
  {
  cout<<"player input position:"<<endl;
  cin>>row>>col;
  cout<< "player place"<<row<<" "<<col<<endl;
  }
else if (options==2)
{
vector< vector<int> > emptypoints;

for (int i=0; i<=n-1;i++)
  for (int j=0;j<=n-1;j++)
    {if (thisboard->board[i][j]=='.')
      {
       vector<int> temp;
       temp.push_back(i);
       temp.push_back(j);
       emptypoints.push_back(temp);
      }
    }
   int randomindex=rand()%emptypoints.size();
  row=  emptypoints[randomindex][0];
  col=  emptypoints[randomindex][1];
  cout<< "random place: "<<row<<" "<<col<<endl;
}

else if (options==3)
{
sttime=time(NULL);  
AIsol= AI.nextstep(thisboard, !aifirst%2==1);
row= AIsol[0];
col= AIsol[1];
cout<< "AI place: "<<AIsol[0]<<" "<<AIsol[1]<<endl;
delete AIsol;
}

tempboard=thisboard->place(row, col);
} while(tempboard==thisboard); // waiting for a legal input

delete thisboard;
thisboard= tempboard;
cout<<endl;
thisboard->show();
if (thisboard->status!=0) break;
//status=thisboard->status;
}   // end while(status=0)

if (thisboard->status==1) {cout<<endl<<"o wins"<<endl;}
else if (thisboard->status==-1) {cout<<endl<<"x wins"<<endl;}
else if (thisboard->status==2) {cout<<endl<<"draw"<<endl;}

} // end while (options != 0)


return 0;
}

