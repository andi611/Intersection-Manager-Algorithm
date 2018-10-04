#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <iomanip>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
using namespace std;
/*
double Bezier(vector<double> &X,int n,double t,vector< vector <double> > &B)
{
	for (int i=0;i<=n-1;i++)
	{
		B[i][i]=X[i];
	}
	for (int j=0;j<=n-2;j++)
	{
		for(int k=0;k<=n-j-2;k++)
			B[k][j+k+1]=(1-t)*B[k][j+k]+t*B[k+1][j+k+1];
	}
	return B[0][n-1];
}
*/
/*
class source{
	
	public:
		vector <int> flow;
		vector <int> x;
		vector <int> y;
		int number=0; 
		void insert_value(int,int,int);
		void sort();
};
void source::sort()
{
	int temp_x,temp_y,posi;
	vector <int> tt_x;
	vector <int> tt_y;
	vector <int> tt_flow;
	temp_x=x[0];
	temp_y=y[0];
	posi=0;
	for(int i=0;i<number;i++)
	{
		for (int j=0;j<number;j++)
		{
			if(x[j]<temp_x)
			{
				temp_x=x[j];
				temp_y=y[j];
				posi=j;
			}
			if(x[j]==temp_x)
			{
				if(y[j]<temp_y)
				{	
					temp_x=x[j];
					temp_y=y[j];
					posi=j;
				}
			}
		}
		tt_x.push_back(temp_x);
		tt_y.push_back(temp_y);
		tt_flow.push_back(flow[posi]);
		x[posi]=2147483647;
		y[posi]=2147483647;
		temp_x=2147483647;
		temp_y=2147483647;
		posi=0;
	}
	for(int i=0;i<number;i++)
	{
		x[i]=tt_x[i];
		y[i]=tt_y[i];
		flow[i]=tt_flow[i];
	}
}
void source::insert_value(int axis_x, int axis_y,int num_flow)
{
	x.push_back(axis_x);
	y.push_back(axis_y);
	flow.push_back(num_flow);
	number+=1;
}
*/
void pop_car(vector< vector <int> > & input_mmat,vector< vector <int> > & output_mmat,vector <int> & who1,vector <int> &qq,vector <int> & stock1,int & num_column,int number)
{
	cout<<number<<"+++"<<endl;
	for(int i=0;i<4;i++)
	{
		if (who1[i]==1)
		{
			output_mmat[i][num_column]=input_mmat[i][qq[i]];
			qq[i]+=1;
			stock1[i]=stock1[i]-1;
		}
		if(who1[i]==0)
		{
			for (int j=qq[i];j<number;j++)
			{
				if(input_mmat[i][j]==0)
				{
					for(int k=j;k<number-1;k++)
					{
						input_mmat[i][k]=input_mmat[i][k+1];
					}
					input_mmat[i][number-1]=0;
					break;
				}
			}
		}
	}
	num_column+=1;
	for(int i=0;i<4;i++)
	{
		who1[i]=0;
	}
}
void evaluate(vector< vector <int> > & input_mmat,vector <int> & who1,vector <int> &qq,vector <int> & stock1)
{
	int count_11=0;
	int max=0,max_position=0;
	vector <int> temp_who=vector <int> (4,0);
	if(input_mmat[0][qq[0]]==1&&input_mmat[1][qq[1]]==1&&input_mmat[2][qq[2]]==1&&input_mmat[3][qq[3]]==1)//for 4 cars 1111
	{
		who1[0]=1;
		who1[1]=1;
		who1[2]=1;
		who1[3]=1;
	}
	else //for 3 cars
	{
		if(input_mmat[1][qq[1]]==1&&input_mmat[2][qq[2]]==1&&input_mmat[3][qq[3]]==1)//X111
		{
			who1[1]=1;
			who1[2]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==1&&input_mmat[2][qq[2]]==1&&input_mmat[3][qq[3]]==1)//1X11
		{
			who1[0]=1;
			who1[2]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==1&&input_mmat[1][qq[1]]==1&&input_mmat[3][qq[3]]==1)//11X1
		{
			who1[0]=1;
			who1[1]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==1&&input_mmat[1][qq[1]]==1&&input_mmat[2][qq[2]]==1)//111X
		{
			who1[0]=1;
			who1[1]=1;
			who1[2]=1;
		}
		else if(input_mmat[1][qq[1]]==2&&input_mmat[2][qq[2]]==1&&input_mmat[3][qq[3]]==1)//X211
		{
			who1[1]=1;
			who1[2]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==1&&input_mmat[2][qq[2]]==2&&input_mmat[3][qq[3]]==1)//1X21
		{
			who1[0]=1;
			who1[2]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==1&&input_mmat[1][qq[1]]==1&&input_mmat[3][qq[3]]==2)//11X2
		{
			who1[0]=1;
			who1[1]=1;
			who1[3]=1;
		}
		else if(input_mmat[0][qq[0]]==2&&input_mmat[1][qq[1]]==1&&input_mmat[2][qq[2]]==1)//211X
		{
			who1[0]=1;
			who1[1]=1;
			who1[2]=1;
		}
		else  //for 2 cars
		{
			if(input_mmat[0][qq[0]]==3&&input_mmat[1][qq[1]]==1)//31XX
			{
				who1[0]=1;
				who1[1]=1;
			}
			else if (input_mmat[1][qq[1]]==3&&input_mmat[2][qq[2]]==1)//X31X
			{
				who1[1]=1;
				who1[2]=1;
			}
			else if (input_mmat[2][qq[2]]==3&&input_mmat[3][qq[3]]==1)//XX31
			{
				who1[2]=1;
				who1[3]=1;
			}
			else if (input_mmat[0][qq[0]]==1&&input_mmat[3][qq[3]]==3)//1XX3
			{
				who1[0]=1;
				who1[3]=1;
			}
			else if (input_mmat[0][qq[0]]==2&&input_mmat[2][qq[2]]==2)//2X2X
			{
				who1[0]=1;
				who1[2]=1;
			}
			else if (input_mmat[1][qq[1]]==2&&input_mmat[3][qq[3]]==2)//X2X2
			{
				who1[1]=1;
				who1[3]=1;
			}
			else if (input_mmat[0][qq[0]]==2&&input_mmat[1][qq[1]]==1)//21XX
			{
				who1[0]=1;
				who1[1]=1;
			}
			else if (input_mmat[0][qq[0]]==2&&input_mmat[2][qq[2]]==1)//2X1X
			{
				who1[0]=1;
				who1[2]=1;
			}
			else if (input_mmat[1][qq[1]]==2&&input_mmat[2][qq[2]]==1)//X21X
			{
				who1[1]=1;
				who1[2]=1;
			}
			else if (input_mmat[1][qq[1]]==2&&input_mmat[3][qq[3]]==1)//X2X1
			{
				who1[1]=1;
				who1[3]=1;
			}
			else if (input_mmat[2][qq[2]]==2&&input_mmat[3][qq[3]]==1)//XX21
			{
				who1[2]=1;
				who1[3]=1;
			}
			else if (input_mmat[0][qq[0]]==1&&input_mmat[2][qq[2]]==2)//1X2X
			{
				who1[0]=1;
				who1[2]=1;
			}
			else if (input_mmat[0][qq[0]]==1&&input_mmat[3][qq[3]]==2)//1XX2
			{
				who1[0]=1;
				who1[3]=1;
			}
			else if (input_mmat[1][qq[1]]==1&&input_mmat[3][qq[3]]==2)//X1X2
			{
				who1[1]=1;
				who1[3]=1;
			}
			else
			{
				//有任意兩個1=>兩個一起走
				for(int i=0;i<4;i++)
				{
					if(input_mmat[i][qq[i]]==1)
					{
						count_11+=1;
						temp_who[i]=1;
					}
				}
				if(count_11==2)
				{
					for(int i=0;i<4;i++)
					{
						who1[i]=temp_who[i];
					}
					count_11=0;
				}
				//
				else///一個一個走
				{
					for(int i=0;i<4;i++)
					{
						if(stock1[0]<=1&&stock1[1]<=1&&stock1[2]<=1&&stock1[3]<=1)
						{
							if(input_mmat[i][qq[i]]>max)
							{
								max=input_mmat[i][qq[i]];
								max_position=i;
							}
						}
						else
						{
							if(input_mmat[i][qq[i]]>max&&stock1[i]>1)
							{
								max=input_mmat[i][qq[i]];
								max_position=i;
							}
						}
					}
					who1[max_position]=1;
				}
				
			}
		}
	}
	
}
int main(int argc ,char**  argv)
{	
	string s1="",s2="",s3="";
	ifstream ifs;
	string line;
	char* input_file,*output_file;
	input_file=argv[1];
	output_file=argv[2];
	ifs.open(input_file);
	ofstream output(output_file);
	int now=0;
	int n=0,column=0;
	getline(ifs,line,'\n');
	n=(line.length()-2)/3;
	vector< vector <int> > input_mat=vector<vector<int> > (4,vector<int>(n,0));
	vector< vector <int> > output_mat=vector<vector<int> > (4,vector<int>(4*n,0));
	vector <int> queue=vector<int>(4,0);
	vector <int> who=vector<int>(4,0);
	vector <int> stock=vector<int>(4,0);
	for (int i=0;i<4;i++)
	{
		for(int j=4;j<line.length();j++)
		{
			
			if (line[j]=='N'||line[j]=='E'||line[j]=='S'||line[j]=='W')
			{
				if(line[j]=='N')
				{
					input_mat[i][now]=i;
				}
				else if(line[j]=='E')
				{
					input_mat[i][now]=(i+3)%4;
				}
				else if(line[j]=='S')
				{
					input_mat[i][now]=(i+6)%4;
				}
				else if(line[j]=='W')
				{
					input_mat[i][now]=(i+9)%4;
				}
				now+=1;
				stock[i]+=1;
			}
			if(line[j]=='0'&&line[j-1]=='0')
			{
				input_mat[i][now]=0;
				now+=1;
			}
		}
		if (i<3)
		{
			getline(ifs,line,'\n');
		}
		now=0;
	}
	/*
	for (int i=0;i<4;i++)
	{
		for(int j=0;j<n;j++)
		{
			output<<input_mat[i][j]<<" ";
		}
		output<<endl;
	}
	for(int i=0;i<4;i++)
	{
			cout<<stock[i]<<" ";
	}
	cout<<endl;
	who[0]=1;
	who[3]=1;
	pop_car(input_mat,output_mat,who,queue,stock,column,n);
	for(int i=0;i<4;i++)
	{
			cout<<stock[i]<<" ";
	}
	cout<<endl;
	for (int i=0;i<4;i++)
	{
		for(int j=0;j<n;j++)
		{
			output<<output_mat[i][j]<<" ";
		}
		output<<endl;
	}
	output<<endl;
	for (int i=0;i<4;i++)
	{
		for(int j=0;j<n;j++)
		{
			output<<input_mat[i][j]<<" ";
		}
		output<<endl;
	}
	//吃data
	*/
	while(stock[0]!=0||stock[1]!=0||stock[2]!=0||stock[3]!=0)
	{
		evaluate(input_mat,who,queue,stock);
		pop_car(input_mat,output_mat,who,queue,stock,column,n);

	}
	for (int i=0;i<4;i++)
	{
		if(i==0)
		{
			output<<"N: ";
			for(int j=0;j<column;j++)
			{
				if(output_mat[i][j]==3)
				{
					output<<"1E";
				}
				else if(output_mat[i][j]==2)
				{
					output<<"1S";
				}
				else if(output_mat[i][j]==1)
				{
					output<<"1W";
				}
				else if(output_mat[i][j]==0)
				{
					output<<"00";
				}
				if(j!=(column-1))
				{
					output<<" ";
				}
			}
			output<<endl;
		}
		else if(i==1)
		{
			output<<"E: ";
			for(int j=0;j<column;j++)
			{
				if(output_mat[i][j]==3)
				{
					output<<"1S";
				}
				else if(output_mat[i][j]==2)
				{
					output<<"1W";
				}
				else if(output_mat[i][j]==1)
				{
					output<<"1N";
				}
				else if(output_mat[i][j]==0)
				{
					output<<"00";
				}
				if(j!=(column-1))
				{
					output<<" ";
				}
			}
			output<<endl;
		}
		else if(i==2)
		{
			output<<"S: ";
			for(int j=0;j<column;j++)
			{
				if(output_mat[i][j]==3)
				{
					output<<"1W";
				}
				else if(output_mat[i][j]==2)
				{
					output<<"1N";
				}
				else if(output_mat[i][j]==1)
				{
					output<<"1E";
				}
				else if(output_mat[i][j]==0)
				{
					output<<"00";
				}
				if(j!=(column-1))
				{
					output<<" ";
				}
			}
			output<<endl;
		}
		else if(i==3)
		{
			output<<"W: ";
			for(int j=0;j<column;j++)
			{
				if(output_mat[i][j]==3)
				{
					output<<"1N";
				}
				else if(output_mat[i][j]==2)
				{
					output<<"1E";
				}
				else if(output_mat[i][j]==1)
				{
					output<<"1S";
				}
				else if(output_mat[i][j]==0)
				{
					output<<"00";
				}
				if(j!=(column-1))
				{
					output<<" ";
				}
			}
			output<<endl;
		}
	}
	cout<<stock[0]<<" "<<stock[1]<<" "<<stock[2]<<" "<<stock[3]<<" ";
	ifs.close();
	output.close();

	return 0;
}