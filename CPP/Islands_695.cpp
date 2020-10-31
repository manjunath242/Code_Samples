/* C++ program to calculate biggest island by area islands in a given matrix
1 represents a grid of land, 0 represents a grid of water

110001101
001110100
111010000

*/

class Islands{
int maxArea=0;
vector<vector<int>> ilands;

public:
    
    Islands(vector<vector<int>>& grid):ilands(grid) {}
    ~Islands(){}
    
    int getMaxArea()
    {
        return maxArea;
    }
    
	void calculatemaxArea()
	{
		for (int i = 0;i<ilands.size();i++)
		{
			for (int j = 0;j<ilands[0].size();j++)
			{
				if ((ilands[i][j] == 0)||(ilands[i][j] == -1)) continue;
				int area = calculateIndividualArea(i, j);
				if (area> maxArea) maxArea = area;
			}
		}
	}

private:
	int calculateIndividualArea(int i, int j)
	{
		int area = 0;
		if ((ilands[i][j] == 0) || (ilands[i][j] == -1)) return 0;
		ilands[i][j] = -1;
		area++;

		if (i<ilands.size() - 1)
			area = area + calculateIndividualArea(i + 1, j);
		if (i>0)
			area = area + calculateIndividualArea(i - 1, j);
		if (j<ilands[0].size() - 1)
			area = area + calculateIndividualArea(i, j + 1);
		if (j>0)
			area = area + calculateIndividualArea(i, j - 1);
		return area;
	}   
    
};

class Solution {
public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        
        if(grid.size()==0) return 0;
        
        Islands lands(grid);
        lands.calculatemaxArea();
        return lands.getMaxArea();
    }
};