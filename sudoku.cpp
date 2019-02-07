#include<iostream>
#include<curses.h>
#include<cstring>
#include<string>
#include<cstring>
#include<random>
#include<ctime>

using namespace std;
using std::default_random_engine;
using std::uniform_int_distribution;

class sudoku
{
    public:
        sudoku(){};
        void run();

    private:

        void generate();
        void draw();
        void play();
        bool judge(int row,int col);
        bool solve(int row,int col);
        void getValid(int &row,int &col);
        bool isInBoundary(int x,int y);
        int getScrX(int x);
        int getScrY(int y);

        int table[9][9];
        int table_answer[9][9];
        int init_total_numbers;
        bool official_table[9][9];
        int TOTAL_MIN,TOTAL_MAX;
        int draw_table_x_start;
        int draw_table_x_end;
        int draw_table_y_start;
        int draw_table_y_end;
};

void sudoku::run()
{
    while(1)
    {
        clear();
        int maxy,maxx;
        getmaxyx(stdscr,maxy,maxx);
        if(has_colors())
        {
            start_color();
            init_pair(1,COLOR_WHITE,COLOR_BLACK);
            attron(COLOR_PAIR(1));
        }
        string choose_level_s = "Please choose a level:";
        string level_s = "1:easy 2:normal 3:hard q:quit";
        mvaddstr(maxy/2,(maxx-choose_level_s.size())/2,choose_level_s.c_str());
        mvaddstr(maxy/2+1,(maxx-level_s.size())/2,level_s.c_str());
        bool flag = false;
        while(1)
        {
            char c = getch();
            switch (c)
            {
                case '1':
                    TOTAL_MAX = 50;
                    TOTAL_MIN = 40;
                    flag = true;
                break;

                case '2':
                    TOTAL_MAX = 40;
                    TOTAL_MIN = 30; 
                    flag = true;
                    break;
                case '3':
                    TOTAL_MAX = 30;
                    TOTAL_MIN = 20;
                    flag = true;
                    break;
                case 'q' :
                case 'Q' :
                    clear();
                    endwin();
                    exit(0);
                default:
                    beep();
                    break;
            }
            if(flag == true)
                break;
        }

        generate();
        draw();
        play();
   }
}

void sudoku::generate()
{   
    int table_t[9][9] = 
    {
     {8,0,0,0,0,0,0,0,0},
     {0,0,3,6,0,0,0,0,0},
     {0,7,0,0,9,0,2,0,0},
     {0,5,0,0,0,7,0,0,0},
     {0,0,0,8,4,5,7,0,0},
     {0,0,0,1,0,0,0,3,0},
     {0,0,1,0,0,0,0,6,8},
     {0,0,8,5,0,0,0,1,0},
     {0,9,0,0,0,0,4,0,0}
    };
    memcpy(table,table_t,sizeof(table));
    solve(0,0);

    default_random_engine e;
    e.seed(time(0));
    uniform_int_distribution<unsigned> 
        total_numbers(TOTAL_MIN,TOTAL_MAX),
        get_number(1,9),
        get_shuffle_times(0,10),
        get_position(0,8);

    memset(official_table,0,sizeof(official_table));
    init_total_numbers = total_numbers(e);

    int shuffle_times = get_shuffle_times(e);
    int shuffle_table[9] = {1,2,3,4,5,6,7,8,9};
    for(int i=0;i<shuffle_times;)
    {
        int srs_pos = get_position(e);
        int dst_pos = get_position(e);
        if(srs_pos==dst_pos)
            continue;
        else
        {
            int t = shuffle_table[srs_pos];
            shuffle_table[srs_pos] = shuffle_table[dst_pos];
            shuffle_table[dst_pos] = t;
            i++;
        }
    }
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<9;j++)
        {
            table[i][j] = shuffle_table[table[i][j]-1];
        }
    }
    memcpy(table_answer,table,sizeof(table));
    for(int i=0;i<81-init_total_numbers;)
    {
        int row = get_position(e);
        int col = get_position(e);
        if(table[row][col] == 0)
            continue;
        else
        {
            table[row][col] = 0;
            official_table[row][col] = 1;
            i++;
        }
    }
}

void sudoku::draw()
{
    clear();
    string wall   = "=====================================";
    string row_s1 = "|   |   |   :   |   |   :   |   |   |";
    string row_s2 = "- - + - + - + - + - + - + - + - + - +";
    string row_s3 = "----+---+---+---+---+---+---+---+---+";
    string row_s4 = "-------------------------------------";
    int len = wall.size();
    int height = 19;
    int scrx,scry;
    getmaxyx(stdscr,scry,scrx);
    draw_table_x_start = (scrx-len) / 2 ;
    draw_table_x_end = draw_table_x_start + len;
    draw_table_y_start = (scry-19) / 2;
    draw_table_y_end = draw_table_y_start + height;
    move(draw_table_y_start,draw_table_x_start);

    if(!has_colors())
    {
        for(int i=0;i<19;i++)
        {
            for(int j=0;j<len;j++)
            {
                if(i==0 || i==18)
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,wall[j]);
                else if(i%2 == 0 && i%3 == 0)
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s3[j]);
                else if(i%2 == 0)
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s2[j]);
                else 
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s1[j]);
            }
        }
    }
    else
    {
        start_color(); // 先start,再init_pair
        init_pair(1,COLOR_RED,COLOR_BLACK);
        init_pair(2,COLOR_WHITE,COLOR_BLACK);
        for(int i=0;i<19;i++)
        {
            for(int j=0;j<len;j++)
            {
                if(i==0 || i==18)
                {
                    attron(COLOR_PAIR(2));
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s4[j]);
                    // attroff(COLOR_PAIR(2));
                }
               else if(i%2 == 0 && i%3 == 0)
               {
                    attron(COLOR_PAIR(1));
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s3[j]);
                    // attroff(COLOR_PAIR(1));
               }
               else if(i%2 == 0)
               {
                    attron(COLOR_PAIR(2));
                    mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s2[j]);
                    // attroff(COLOR_PAIR(2));
               }
               else 
               {
                    if(row_s1[j]==':')
                    {
                        attron(COLOR_PAIR(1));
                        mvaddch(draw_table_y_start+i,draw_table_x_start+j,'|');
                        // attroff(COLOR_PAIR(1));
                    }
                    else 
                    {
                        attron(COLOR_PAIR(2));
                        mvaddch(draw_table_y_start+i,draw_table_x_start+j,row_s1[j]);
                        // attroff(COLOR_PAIR(2));
                    }
               }
            }
        }
    }
   for(int i=0;i<9;i++) 
    {
        for(int j=0;j<9;j++)
        {
            int x = getScrX(j);
            int y = getScrY(i);
            if(table[i][j]!=0)
                mvaddch(y,x,table[i][j]+'0');
            else
                mvaddch(y,x,' ');
        }
    }
    string s1 = "VIM STYLE -- Up:k Down:j Left:h Right:l";
    string s2 = "NORMAL STYLE -- Up:w Down:s Left:a Right:d";
    string s3 = "Quit:q Restart:r Answer:z Hint:?";
    mvaddstr(draw_table_y_end+1,(getmaxx(stdscr) - s1.size())/2,s1.c_str());
    mvaddstr(draw_table_y_end+2,(getmaxx(stdscr) - s2.size())/2,s2.c_str());
    mvaddstr(draw_table_y_end+3,(getmaxx(stdscr) - s3.size())/2,s3.c_str());
}

void sudoku::play()
{
    int maxy,maxx;
    getmaxyx(stdscr,maxy,maxx);
    int table_row=0,table_col=0;
    move(getScrY(table_row),getScrX(table_col));
    int to_fill = 81 - init_total_numbers;
    while(1)
    {
        char key = getch();
        int y,x;
        getyx(stdscr,y,x);
        if(key == 'w' || key == 'W' || key == 'k' || key == 'K')
        {
            if(isInBoundary(table_row-1,table_col))
            {
                table_row--;
                move(y-2,x);
            }
        }
        else if(key == 'a' || key == 'A' || key == 'h' || key == 'H')
        {
            if(isInBoundary(table_row,table_col-1))
            {
                table_col--;
                move(y,x-4);
            }
        }
        else if(key == 's' || key == 'S' || key == 'j' || key == 'J')
        {
            if(isInBoundary(table_row+1,table_col))
            {
                table_row++;
                move(y+2,x);
            }
        }
        else if(key == 'd' || key == 'D' || key == 'l' || key == 'L')
        {
            if(isInBoundary(table_row,table_col+1))
            {
                table_col++;
                move(y,x+4);
            }
        }
        else if(key == 'r' || key == 'R')
        //返回到选难度界面
            return ;
        else if(key == 'q' || key == 'Q')
        {
            clear();
            endwin();
            exit(0);
        }
        else if(key == 'z' || key =='Z')
        {
            clear();
            solve(0,0);
            draw();
        }
        else if(key == '?')
        {
            int ans = table_answer[table_row][table_col];
            table[table_row][table_col] = ans;
            addch(ans+'0');
            move(y,x);
            to_fill--;
        }
        else if( key>='1' && key<='9' || key==' ')//空格键消除        
        {//生成的数不允许覆盖
            if(official_table[table_row][table_col]==0)
                continue;
            else
            {
                table[table_row][table_col] = key-'0';
                addch(key);
                move(y,x); //填完后光标复原
                //jugde
                to_fill--;
            }
        }
        else
            beep();
        
        if(to_fill == 0)
            return;
    }
}

void sudoku::getValid(int &row,int &col)
{
    while(col<=9 && row<9 )
    {
        if(col==9)
        {
            row++;
            col=0;
        }
        if(row==9)
            return;
        if(table[row][col]!=0)
            col++;
        else
            break;
    }
}

bool sudoku::solve(int row,int col)
{
    getValid(row,col);
    if(row==9)
        return true;
    for(int i=1;i<=9;i++)
    {
        table[row][col] = i;
        if(judge(row,col))
        {
            if(solve(row,col+1))
                return true;
        }
    }
    table[row][col] = 0;
    return false;
}

bool sudoku::judge(int row,int col)
{
    //check row
    for(int i=0;i<9;i++)
    {
        if(i!=col && table[row][i]==table[row][col])
            return false;
    }
    //check column
    for(int i=0;i<9;i++)
    {
        if(i!=row && table[i][col]==table[row][col])
            return false;
    }
    //check box 
    int box_row_start = (row/3)*3;
    int box_col_start = (col/3)*3;
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            if(box_row_start+i!=row 
            && box_col_start+j!=col 
            && table[box_row_start+i][box_col_start+j]==table[row][col])
                return false;
        }
    }
    return true;
}

int sudoku::getScrX(int x)
{
    return draw_table_x_start + (x+1)*4 - 2;
}

int sudoku::getScrY(int y)
{
    return draw_table_y_start + (y+1)*2 - 1;
}

bool sudoku::isInBoundary(int row,int col)
{
    if(row>=0 && row<=8 && col>=0 && col<=8)
        return true;
    else
        return false;
}

void init()
{
    initscr();
    cbreak();
    noecho();
    int maxx,maxy;
    getmaxyx(stdscr,maxy,maxx);
    string welcome = "Welcome to Sudoku!";
    string press_s ="Press any key to start";
    string author_s = "Author:sunriz";
    mvaddstr(maxy/2,(maxx-welcome.size())/2,welcome.c_str());
    mvaddstr(maxy/2+1,(maxx-press_s.size())/2,press_s.c_str()); 
    mvaddstr(maxy/2+2,(maxx-author_s.size())/2,author_s.c_str()); 
    getch();
    //不能用getchar() -- 刷新屏幕并读输入
}

int main()
{    
    init();
    sudoku game;
    game.run();
    endwin();
}
