class Islands{
    
    public:
    vector<vector<char>> islands;
    int numIslands=0;
    int r,c;
    
    Islands(vector<vector<char>> & grid): islands(grid), r(grid.size()), c(grid[0].size())
    {
    }
    
    ~Islands(){}
    
    void calculateislandCount()
    {
       for(int i=0;i<islands.size();i++)
        {
            for(int j=0;j<=islands[0].size();j++)
            {
                if(islands[i][j]=='1')
                {
                    markConnectedLands(i,j);
                    numIslands++;
                }
            }
        }   
    }
    
    void markConnectedLands(int i,int j)
    {
        if(islands [i][j]!='1') return;
        else{
            islands[i][j]='-';
            if((j+1)<c) markConnectedLands(i, j+1);
            if((i+1)<r) markConnectedLands(i+1, j);
            
            return;
        }
        return;
    }
    
    int getIslandCounts()
    {
        return numIslands;
    }
    
};

class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        if (grid.size()==0) return 0;
        
        Islands lands(grid);
        lands.calculateislandCount();
        return lands.getIslandCounts();
        
    }
};