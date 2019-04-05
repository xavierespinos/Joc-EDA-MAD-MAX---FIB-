#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME XESPINOS


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    vector<int> pos_x = {1,-1,1,-1,-1,1,0,0};
    vector<int> pos_y = {0,0,1,1,-1,-1,1,-1};
    vector<Dir> dir = {Bottom,Top,BR,RT,TL,LB,Right,Left};

    vector<vector<int>> mapa_aigua;
    vector<vector<int>> mapa_gas;
    vector<vector<int>> mapa_ciutat;

    bool ok_cerca(int x, int y, vector<vector<int>>& mapa){

        return (pos_ok(x,y) and cell(x, y).type != Wall and cell(x,y).type != Station
        and cell(x,y).type != Water and cell(x,y).type != City and mapa[x][y] == 999);
    }

    bool ok_warrior(int x, int y){

        return (pos_ok(x,y) and cell(x, y).type != Wall and cell(x,y).type != Station
        and cell(x,y).type != Water and cell(x,y).id == -1);
    }

    bool ok_cotxe(int x, int y){

        return (pos_ok(x,y) and cell(x, y).type != Wall and cell(x,y).type != Station
        and cell(x,y).type != Water and cell(x,y).type != City);
    }

    bool ok_water(int x, int y, vector<vector<int>>& mapa){

        return (pos_ok(x,y) and cell(x, y).type != Wall and cell(x,y).type != Station
        and cell(x,y).type != Water and mapa[x][y] == 999);
    }

    bool ok(int x, int y){

        return (pos_ok(x,y) and cell(x, y).type != Wall and cell(x,y).type != Station
        and cell(x,y).type != Water);
    }

    
    vector<vector<int>> crea_mapa(int x){

        vector<vector<int>> mapa(60, vector<int>(60, 999));
        queue<pair<int,int>> q;
        
        for(int i = 0; i < 60; ++i){
            for(int j = 0; j < 60; ++j){
                if(x == 0 and cell(i,j).type == Station) {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                if(x == 1 and cell(i,j).type == Water)  {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                if(x == 2 and cell(i,j).type == City and cell(i,j).owner != me())  {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                if(x == 3 and cell(i,j).type == City)  {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                if(x == 4 and cell(i,j).type == City and cell(i,j).owner == me())  {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                if(x == 5 and cell(i,j).type == Road)  {
                    q.push({i,j});
                    mapa[i][j] = 0;
                }
                 
            }
        }
        
        while(not q.empty()){
            pair<int, int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            q.pop();

            for(int k = 0; k < 8; ++k){
                if(x==0){
                    if(ok_cerca(x+pos_x[k],y+pos_y[k],mapa)){
                        q.push({x+pos_x[k],y+pos_y[k]});
                        mapa[x+pos_x[k]][y+pos_y[k]] = mapa[x][y] + 1;
                    }
                }
                else {
                    if(ok_water(x+pos_x[k],y+pos_y[k],mapa)){
                        q.push({x+pos_x[k],y+pos_y[k]});
                        mapa[x+pos_x[k]][y+pos_y[k]] = mapa[x][y] + 1;
                    }

                }
            }       
        }

        return mapa;

    }

    pair<int,int> trobar_enemic(int x0, int y0){

        vector<vector<bool>> mapa(60, vector<bool>(60,false));
        queue<pair<int,int>> q;
        q.push({x0,y0});
        mapa[x0][y0] = true;
        int temp = 0;

        while(not q.empty()){
            pair<int,int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            Cell c = cell(x,y);
            q.pop();
            ++temp;

            if(c.id != -1 and unit(c.id).player !=me() and unit(c.id).type == Warrior) return punt;

            mapa[x][y] = true;

            for(int k = 0; k < 8; ++k){
                int xp = x+pos_x[k];
                int yp = y+pos_y[k];
                if(ok_cotxe(xp, yp) and mapa[xp][yp] == false) {
                    q.push({xp,yp});
                    mapa[xp][yp] = true;
                }
            }
        }

        return {-1,-1};
    }

    pair<int,int> trobar_ciutat(int x0, int y0){

        vector<vector<bool>> mapa(60, vector<bool>(60,false));
        queue<pair<int,int>> q;
        q.push({x0,y0});
        mapa[x0][y0] = true;
        int temp = 0;

        while(not q.empty()){
            pair<int,int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            Cell c = cell(x,y);
            q.pop();
            ++temp;

            if(c.type == City and c.owner != me()) return punt;
            mapa[x][y] = true;

            for(int k = 0; k < 8; ++k){
                int xp = x+pos_x[k];
                int yp = y+pos_y[k];
                if(ok(xp, yp) and mapa[xp][yp] == false) {
                    q.push({xp,yp});
                    mapa[xp][yp] = true;
                }
            }
        }

        return {-1,-1};

    }

    int water(int x0, int y0){

        vector<vector<bool>> mapa(60, vector<bool>(60,false));
        vector<vector<int>> dist(60, vector<int>(60,0));
        queue<pair<int,int>> q;
        q.push({x0,y0});
        mapa[x0][y0] = true;
        

        while(not q.empty()){
            pair<int,int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            Cell c = cell(x,y);
            q.pop();
            

            if(c.type == Water) return dist[x][y];
            mapa[x][y] = true;

            for(int k = 0; k < 8; ++k){
                int xp = x+pos_x[k];
                int yp = y+pos_y[k];
                if(ok(xp, yp) and mapa[xp][yp] == false) {
                    q.push({xp,yp});
                    mapa[xp][yp] = true;
                    dist[xp][yp] = dist[x][y] + 1;
                }
            }
        }

        return {15};

    }

    int gasolina(int x0, int y0){

        vector<vector<bool>> mapa(60, vector<bool>(60,false));
        vector<vector<int>> dist(60, vector<int>(60,0));
        queue<pair<int,int>> q;
        q.push({x0,y0});
        mapa[x0][y0] = true;
        

        while(not q.empty()){
            pair<int,int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            Cell c = cell(x,y);
            q.pop();
            

            if(c.type == Station) return dist[x][y];
            mapa[x][y] = true;

            for(int k = 0; k < 8; ++k){
                int xp = x+pos_x[k];
                int yp = y+pos_y[k];
                if(ok(xp, yp) and mapa[xp][yp] == false) {
                    q.push({xp,yp});
                    mapa[xp][yp] = true;
                    dist[xp][yp] = dist[x][y] + 1;
                }
            }
        }

        return {15};

    }

    pair<int,int> trobar_cotxe(int x0, int y0){

        vector<vector<bool>> mapa(60, vector<bool>(60,false));
        queue<pair<int,int>> q;
        q.push({x0,y0});
        mapa[x0][y0] = true;
        int temp = 0;

        while(not q.empty()){
            pair<int,int> punt = q.front();
            int x = punt.first;
            int y = punt.second;
            Cell c = cell(x,y);
            q.pop();
            ++temp;

            if(c.id != -1 and unit(c.id).player !=me() and unit(c.id).type == Car) return punt;
            mapa[x][y] = true;

            for(int k = 0; k < 8; ++k){
                int xp = x+pos_x[k];
                int yp = y+pos_y[k];
                if(ok(xp, yp) and mapa[xp][yp] == false) {
                    q.push({xp,yp});
                    mapa[xp][yp] = true;
                }
            }
        }

        return {-1,-1};

    }

    void move_warriors(){
        
        if(round()%4 != me()) return;

        vector<int> w;
        w = warriors(me());

        for(int id : w){
            
            Pos p = unit(id).pos;
            int x = p.i;
            int y = p.j;
            int need_water = water(x,y);

            if(unit(id).water < need_water){

                int pos_min = -1;
                int min;
                int xp;
                int yp;

                for(int k = 0; k < 8; ++k){
                    xp = x+pos_x[k];
                    yp = y+pos_y[k];
                    if(ok_warrior(xp,yp) and mapa_aigua[xp][yp] < mapa_aigua[x][y]){
                        min = mapa_aigua[xp][yp];
                        pos_min = k;
                    }
                }
                
                if(pos_min != -1) command(id, dir[pos_min]);

            }

            else if(cell(p).type != City or (round() <= 20 and id%3 != 0)) {
                pair<int,int> ciutat = trobar_ciutat(x,y);                
                
                Pos cotxe = {-1,-1};

                for (int k = 0; k < 8; ++k){
                    int xp = x+pos_x[k];
                    int yp = y+pos_y[k];
                    if(pos_ok(xp,yp) and cell(xp,yp).type != Wall and cell(xp,yp).type != Water
                    and cell(xp,yp).type != Station and cell(xp,yp).id > - 1 and
                    unit(cell(xp,yp).id).player != me() and unit(cell(xp,yp).id).type == Car){
                        cotxe = {xp,yp};
                    }
                }

                if(cotxe.i != -1){
                    int i = cotxe.i;
                    int j = cotxe.j;
                    Dir k = None;
                    if (i<x and j<y and ok_warrior(x+1,y+1)) k = BR;
                    else if (i<x and j==y and ok_warrior(x+1,y)) k = Bottom;
                    else if (i<x and j>y and ok_warrior(x+1,y-1)) k = LB;
                    else if (i>x and j<y and ok_warrior(x-1,y+1)) k = RT;
                    else if (i>x and j==y and ok_warrior(x-1,y)) k = Top;
                    else if (i>x and j>y and ok_warrior(x-1,y-1)) k = TL;
                    
                    if (k != None) command(id, k);

                }

            
                else{

                    int i = ciutat.first;
                    int j = ciutat.second;
                    int dist;
                    int min = 999999;
                    int pos_min = -1;

                    if(i != -1 and j != -1){
                        for(int k = 0; k < 8; ++k){
                            int xp = x+pos_x[k];
                            int yp = y+pos_y[k];
                            if((ok(xp,yp) and cell(xp,yp).id == -1) or (cell(xp,yp).id > -1 and 
                            unit(cell(xp,yp).id).player != me() and unit(cell(xp,yp).id).type == Warrior)){
                                dist = (i-xp)*(i-xp)+(j-yp)*(j-yp);
                                if (dist < min){
                                    min = dist;
                                    pos_min = k;
                                }
                            }
                        }
                        if(pos_min != -1) command(id, dir[pos_min]);
                    }

                }
                
            }  
        }
    }

    void move_cars(){

        vector<int> c;
        c = cars(me());
        set<Pos> posicions;

        for(int id : c){

            Pos p = unit(id).pos;
            int x = p.i;
            int y = p.j;
            int need_gasolina = gasolina(x,y);

            if(unit(id).food < need_gasolina and can_move(id)){

                int pos_min = -1;
                int min;
                int xp;
                int yp;
                int xmin;
                int ymin;

                for(int k = 0; k < 8; ++k){
                    xp = x+pos_x[k];
                    yp = y+pos_y[k];
                    if(ok_cotxe(xp,yp) and mapa_gas[xp][yp] < mapa_gas[x][y]){
                        if(((cell(xp,yp).id > -1 and unit(cell(xp,yp).id).player != me() and 
                        unit(cell(xp,yp).id).type == Warrior) or cell(xp,yp).id == -1)){
                            min = mapa_gas[xp][yp];
                            pos_min = k;
                            xmin = xp;
                            ymin = yp;
                        }
                    }
                }

                set<Pos>::iterator it = posicions.find({xmin,ymin});
                if (it == posicions.end()) {
                    posicions.insert({xmin,ymin});
                    if (pos_min != -1) command(id, dir[pos_min]);
                }
            }

            else if (can_move(id)){

                pair<int,int> enemic;
                if(round() >= 8) enemic = trobar_enemic(x,y);
                else enemic = trobar_ciutat(x,y);
                int i = enemic.first;
                int j = enemic.second;
                int dist;
                int min = 9999999;
                int pos_min = -1;
                int xp;
                int yp;
                int xmin;
                int ymin;

                if(i != -1){

                    for(int k = 0; k < 8; ++k){
                        xp = x+pos_x[k];
                        yp = y+pos_y[k];
                         
                        int carr = 0;
                        if(ok_cotxe(xp,yp)){
                            if((cell(xp,yp).id == -1) or (cell(xp,yp).id > - 1 and 
                            unit(cell(xp,yp).id).player != me() and unit(cell(xp,yp).id).type == Warrior)){
                                if (cell(xp,yp).type == Road) carr = 1;
                                dist = (i-xp)*(i-xp)+(j-yp)*(j-yp) - carr;
                                if (dist < min){
                                    min = dist;
                                    pos_min = k;
                                    xmin = xp;
                                    ymin = yp;
                                }
                            }
                        }
                    }
                    
                    set<Pos>::iterator it = posicions.find({xmin,ymin});
                    if (it == posicions.end()) {
                        posicions.insert({xmin,ymin});
                        if (pos_min != -1) command(id, dir[pos_min]);
                    }
                }


            }
        }
             


    }

    /**
     * Types and attributes for your player can be defined here.
     */

    /**
     * Play method, invoked once per each round.
     */
    virtual void play () {

        if(status(me()) > 0.95) return;
        
        if(round() == 0){   
            mapa_aigua = crea_mapa(1);
            mapa_gas = crea_mapa(0);
            mapa_ciutat = crea_mapa(3);
        }
        move_warriors();
        move_cars();

        
       
    }

    };


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
