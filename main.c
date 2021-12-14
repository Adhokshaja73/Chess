#include <gtk/gtk.h>
#include <windows.h>
#include <string.h>
#define MAX 50  //max number of characters in path

//DataStructures and Global Variables

char assets[MAX];  //String that holds path of directory which contains images of pieces
GtkWidget *window;  //gtkWindow
GtkWidget *grid;    //Grid to hold pieces

typedef struct
{
  char path[MAX]; //path of image file to load. set to "" if cell is empty
  int x;          //position index X
  int y;          //position index Y
  int pos;        //position number:  a value between 0-63 (for 64 cells)
  int pieceColor; //color of the piece, set to 2 if cell is empty, set to 0 for black pieces 1 for white pieces
  char pieceName; //name of the peice. r = rook, h = knight, b = bishop, q = queen, k = king
} cell;

cell positions[64]; //array of 64 cells to store data of the entire board
cell *prevCell;     //pointer to previously clicked cell
int allowedPos[64]; //stores the possible positions of a selected piece based on rules of the game
int allowedPosSize;
//This like a flag to check if the player is clicking first time or second time
//clicking first time selects the piece, second time moves the piece
enum firstClick
{
  FIRST,
  SECOND
};
//variable to determine who's turn it is
enum mTurn
{
  B,
  W
};

enum mTurn turn = W; //initialized to W to say white plays first
enum firstClick click = FIRST; //first click initialized to FIRST

//Paths to images of pieces
char *WhitePieces[8] = {"r_w.png","h_w.png","b_w.png","q_w.png","k_w.png","b_w.png","h_w.png","r_w.png"};
char *BlackPieces[8] = {"r_b.png","h_b.png","b_b.png","q_b.png","k_b.png","b_b.png","h_b.png","r_b.png"};

//FUNCTION DECLARATIONS:

//function to handle click events
void onClick(GtkWidget *widget, GdkEvent event, gpointer);
//function to initialize board
void board();
//function to create/modify cells
GtkWidget *box(char piece[10], int mPos, int mX, int mY, int pieceColor,int bg);
//function to get allowed destination positions for a selected piece
void getAllowedPos(int flag);
//function to check if position in in allowedPosition array
int checkInArray(int pos);
//returns position taking i and j
int getPos(int i, int j);
//Show dialog
void showDialog(GtkWidget *widget, gpointer window, char *title,char *message);
//stores data about a cell

int main(int argc, char *argv[])
{
  //initialize gtk
  gtk_init(NULL, NULL);
  //get current working directory (works only for windows)
  GetCurrentDirectory(50, assets);
  //replacing all \ in the path to / using ascii values
  for (int i = 0; i < 50; i++)
  {
    if (assets[i] == 92)
    {
      assets[i] = 47;
    }
  }
  //get path of the directory where the images are stored and keep it in variable assets
  strcat(assets, "/assets/");

  //initialize the board
  board();
  //run gtk
  gtk_main();

  return 0;
}


GtkWidget *box(char piece[10], int mPos, int mX, int mY, int pieceColor,int bg)
{
    /*
    This function takes in path to image of a piece (piece), x, y, pos, color of piece, background color
    pass piece = "", pieceColor = 2 for empty cells
    bg used to determine background color of the cell.
    */
  GtkWidget *image;
  GtkWidget *box;
  GdkRGBA c;
  //if bg = 0, set color = light for even numbered cells and color = dark for odd numbered cells
  if(bg == 0)
    {
    if ((mX + mY) % 2 == 1){
        c.alpha = 1;
        c.red = 0.72;
        c.green = 0.54;
        c.blue = 0.28;
    }
    else {
       c.alpha = 1;
        c.red = 0.89;
        c.green = 0.756;
        c.blue = 0.435;
    }
  }
  //if bg is 1 set color to highlight
  else if(bg == 1){
       c.alpha = 1;
        c.red = 0.8;
        c.green = 0.717;
        c.blue = 0.682;
  }
  //bg > 1 for highlighting targets in red
  else{
    c.alpha = 0.50;
    c.red = 1;
    c.green = 0.2;
    c.blue = 0.2;
  }

  char temp[MAX];
  //create a new gtk event box
  box = gtk_event_box_new();

  //add the pos, x, y, piece color to the array of cells : positions
  positions[mPos].pos = mPos;
  positions[mPos].x = mX;
  positions[mPos].y = mY;
  positions[mPos].pieceColor = pieceColor;

  //if piece is not "" (ie non empty cell)
  if (strcmp(piece, ""))
  {
    //set piece name
    positions[mPos].pieceName = piece[0];
    //create a new gtk image and add the file in path to that
    strcpy(temp, assets);
    image = gtk_image_new_from_file(strcat(assets, piece));
    strcpy(assets, temp);
  }
  //create an empty image view
  else
  {
    positions[mPos].pieceName = '\0';
    image = gtk_image_new();
  }
  //store path in the cells array : postions
  strcpy(positions[mPos].path, piece);

  //set color of box
  gtk_widget_override_background_color(image, 0, &c);
  //add image view to the event box
  gtk_container_add(G_OBJECT(box), image);
  //specify onClick method to the event box
  g_signal_connect(G_OBJECT(box), "button_press_event", G_CALLBACK(onClick), &positions[mPos]);
  //return the box
  return box;
}

void board(void)
{
 /*
    This function is used to initialize the board. This creates a board with pieces in their right places
 */
  GtkWidget *item;
  //initialize gtk window and grid
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Board");
  grid = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER(window), grid);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_spacing(GTK_GRID(grid), 10);

  //loop to iterate 64 times for 64 cells
  //every iteration adds a new cell to the grid
  int count = 0;
  char *piece;
  int pieceColor;
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      //assign piece and piece color based on i and j values
      //to get the pieces in right cells and empty cells in right places
      switch (i)
      {
      case 0:
        //Black Pieces
        pieceColor = 0;
        piece =  BlackPieces[j];
        break;
      case 1:
      	//Black Pawn
        pieceColor = 0;
        piece = "p_b.png";
        break;
      case 6:
        //white pawns
        pieceColor = 1;
        piece = "p_w.png";
        break;
      case 7:
      	//White Pieces
        pieceColor = 1;
       	piece = WhitePieces[j];
        break;
      default:
        piece = "\0";
        pieceColor = 2;
        break;
      }
      //call the box function and pass the set variables and get a GtkWidget returned store it in item
      item = box(piece, count, j, i, pieceColor,0);
      //add item to the grid in right place
      gtk_grid_attach(GTK_GRID(grid), item, j * 10, i * 10, 10, 10);
      //this makes it responsive
      gtk_widget_set_hexpand(item, TRUE);
      gtk_widget_set_vexpand(item, TRUE);
      count += 1;
    }
  }
  gtk_widget_show_all(window);
}

//the parameters should be like these for Gtk onclick events.
//widget tells which widget was clicked,
//data is a pointer that is specified while setting clickListeners to the widget
//We are taking the pointer to the cell in the positions list.
void onClick(GtkWidget *widget, GdkEvent event, gpointer data)
{
  //gets the struct currentCell
  cell *currentCell = data;
  //First click : check who's turn and highlight the possible path
  if (click == FIRST)
  {
    //check if clicked cell is not empty. Do nothing if empty cells are clicked
    if (strcmp(currentCell->path, "")){
     //check who's turn it is
      if (turn == currentCell->pieceColor)
      {
        //update value of click so that next time the button gets clicked, SECOND part is executed
        click = SECOND;
        //store currently selected cell for further reference in SECOND click
        prevCell = currentCell;
        //get and store the possible moves for the selected piece
        getAllowedPos(1);
        if(allowedPosSize == 0){
            click = FIRST;
        }
        //highlight the cell
        else{
            widget = box(currentCell->path, currentCell->pos, currentCell->x, currentCell->y, currentCell->pieceColor,1);
            gtk_grid_attach(GTK_GRID(grid), widget, 10 * (currentCell->x), 10 * (currentCell->y), 10, 10);
            gtk_widget_set_hexpand(widget, TRUE);
            gtk_widget_set_vexpand(widget, TRUE);
            gtk_widget_show_all(window);
            gtk_main();
        }
      }
    }
  }
  //Second click: update the cell or ignore based on contents of clicked cell
  else if (click == SECOND)
  {

    //Check if someone won
    char *title, *message;
    title = "Game Over..!";
    if(prevCell->pieceColor == W && currentCell->pieceColor == B && currentCell->pieceName == 'k' ){
        message = "White won..!";
        showDialog(widget,window,title,message);
    }
    else if(prevCell->pieceColor == B && currentCell->pieceColor == W && currentCell->pieceName == 'k'){
        message = "Blck won..!";
        showDialog(widget,window,title,message);
    }
    //Check if the selected cell is empty or contains a piece that is of other color
    //Change selected piece if clicked on cell with piece of same color
    if(prevCell->pieceColor == currentCell->pieceColor){
        click = FIRST;
        widget = box(prevCell->path, prevCell->pos, prevCell->x, prevCell->y, prevCell->pieceColor,0);
        gtk_grid_attach(GTK_GRID(grid), widget, 10 * (prevCell->x), 10 * (prevCell->y), 10, 10);
        gtk_widget_set_hexpand(widget, TRUE);
        gtk_widget_set_vexpand(widget, TRUE);
        for(int i = 0; i < allowedPosSize; i++){
            int tempPos = allowedPos[i];
            if(positions[tempPos].pieceName == 'd'){
                widget = box("", positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,0);
            }
            else{
                widget = box(positions[tempPos].path, positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,0);
            } gtk_grid_attach(GTK_GRID(grid), widget, 10 * (positions[tempPos].x), 10 * (positions[tempPos].y), 10, 10);
            gtk_widget_set_hexpand(widget, TRUE);
            gtk_widget_set_vexpand(widget, TRUE);
        }
        gtk_widget_show_all(window);

    }
    if (strcmp(currentCell->path, "")){
      //check if currentCell->pos is in allowedPos array
      if(checkInArray(currentCell->pos)){
      //update click so that next time FIRST part is executed
      click = FIRST;
      //update the turn so other player gets to play next
      if (turn == B){
        turn = W;
      }
      else if (turn == W){
        turn = B;
      }
      //promote pawn to queen if it reaches extreme end
      if(prevCell->y == 1 && prevCell->pieceColor == W && prevCell->pieceName == 'p'){
            strcpy(prevCell->path,"q_w.png");
            prevCell->pieceName = "q";
      }
      if(prevCell->y == 6 && prevCell->pieceColor == B && prevCell->pieceName == 'p'){
            strcpy(prevCell->path,"q_b.png");
            prevCell->pieceName = "q";
      }
      char *temp;
      strcpy(temp,assets);
      if(prevCell->pieceColor != currentCell->pieceColor && currentCell->pieceColor != 2){
        //hit piece sound
        PlaySound(strcat(assets,"sound_2.wav"),NULL,SND_ASYNC);
      }
      else{
            PlaySound(strcat(assets,"sound.wav"),NULL,SND_ASYNC);
      }
      strcpy(assets,temp);
      //add the image in previous cell to current cell
      widget = box(prevCell->path, currentCell->pos, currentCell->x, currentCell->y, prevCell->pieceColor,0);
      gtk_grid_attach(GTK_GRID(grid), widget, 10 * (currentCell->x), 10 * (currentCell->y), 10, 10);
      gtk_widget_set_hexpand(widget, TRUE);
      gtk_widget_set_vexpand(widget, TRUE);
      //make the previous cell empty
      widget = box("", prevCell->pos, prevCell->x, prevCell->y, 2,0);
      gtk_grid_attach(GTK_GRID(grid), widget, 10 * (prevCell->x), 10 * (prevCell->y), 10, 10);
      gtk_widget_set_hexpand(widget, TRUE);
      gtk_widget_set_vexpand(widget, TRUE);

      //remove highlights
      for(int i = 0; i < allowedPosSize; i++){
        int tempPos = allowedPos[i];
        if(positions[tempPos].pieceName == 'd'){
            widget = box("", positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,0);
        }
        else{
            widget = box(positions[tempPos].path, positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,0);
        } gtk_grid_attach(GTK_GRID(grid), widget, 10 * (positions[tempPos].x), 10 * (positions[tempPos].y), 10, 10);
        gtk_widget_set_hexpand(widget, TRUE);
        gtk_widget_set_vexpand(widget, TRUE);
        }
      gtk_widget_show_all(window);

      prevCell = currentCell;
      getAllowedPos(0);
      gtk_main();
     }
    }

  }
}

void showDialog(GtkWidget *widget, gpointer window,char *title,char *message) {

  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            message);
  gtk_window_set_gravity(G_OBJECT(dialog),GDK_GRAVITY_SOUTH);
  gtk_widget_set_size_request(G_OBJECT(dialog),255,255);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void getAllowedPos(int flag){
    char pieceName = prevCell->pieceName;
    int pieceCol  = prevCell->pieceColor;
    int x = prevCell->x;
    int y = prevCell->y;
    allowedPosSize = 0;
    int allowedXY[32][2];
    int xySize;
    xySize = 0;    //define rules and add the positions where the selected pieces can go
    switch(pieceName){
    //rules for a pawn
        case 'p':
            //first turn : allow pawn to move 2 steps
             if((y == 1 && pieceCol == B)|| (y == 6 && pieceCol == W) && y+1 < 8 && y-1>=0){
                //allow 2 places for first move
                //for white piece
                if(pieceCol){
                    if(positions[getPos(x,y-1)].pieceColor == 2){
                        allowedXY[0][0] = x;
                        allowedXY[0][1] = y-1;
                        xySize = 1;
                        if(positions[getPos(x,y-2)].pieceColor == 2){
                        allowedXY[1][0] = x;
                        allowedXY[1][1] = y-2;
                        xySize = 2;
                        }
                    }
                }
                //for black piece
                else{
                    if(positions[getPos(x,y+1)].pieceColor == 2 && y+1 < 8 && y-1>=0){
                        allowedXY[0][0] = x;
                        allowedXY[0][1] = y+1;
                        xySize = 1;
                        if(positions[getPos(x,y+2)].pieceColor == 2){
                        allowedXY[1][0] = x;
                        allowedXY[1][1] = y+2;
                        xySize = 2;
                        }
                    }
                }
            }
            //later moves : allow only one step
            else{
                //allow only one place
                //for white piece
                if(pieceCol){
                    if(positions[getPos(x,y-1)].pieceColor == 2){
                        allowedXY[0][0] = x;
                        allowedXY[0][1] = y-1;
                        xySize = 1;
                    }
                }
                //for black piece
                else{
                    if(positions[getPos(x,y+1)].pieceColor == 2){
                        allowedXY[0][0] = x;
                        allowedXY[0][1] = y+1;
                        xySize = 1;
                    }
                }
            }
            //check diagonal position for enemy piece
            if(pieceCol == W){
                //white piece, check x+1,y-1 and x-1,y-1 for black
                if(positions[getPos(x-1,y-1)].pieceColor == B && y-1>=-1 && x-1>=0){
                    allowedXY[xySize][0] = x-1;
                    allowedXY[xySize][1] = y-1;
                    xySize += 1;
                }
                if(positions[getPos(x+1,y-1)].pieceColor == B && y-1>=-1 && x+1<8){
                    allowedXY[xySize][0] = x+1;
                    allowedXY[xySize][1] = y-1;
                    xySize += 1;
                }
            }
            else{
                //black piece, check x+1,y+1 and x-1,y+1 for white
                if(positions[getPos(x+1,y+1)].pieceColor == W && y+1<8 && x+1<8){
                    allowedXY[xySize][0] = x+1;
                    allowedXY[xySize][1] = y+1;
                    xySize += 1;
                }
                if(positions[getPos(x-1,y+1)].pieceColor == W && y+1<8 && x-1>=0){
                    allowedXY[xySize][0] = x-1;
                    allowedXY[xySize][1] = y+1;
                    xySize += 1;
                }
            }
            break;
        case 'h':
            xySize = 0;
            if(x+1<8 && y+2<8 && x+1 >= 0 && y+2 >= 0){
                allowedXY[xySize][0] = x+1;
                allowedXY[xySize][1] = y+2;
                xySize += 1;
            }
            if(x-1<8 && y+2<8 && x-1 >= 0 && y+2 >= 0){
                allowedXY[xySize][0] = x-1;
                allowedXY[xySize][1] = y+2;
                xySize += 1;
            }
            if(x+1<8 && y-2<8 && x+1 >= 0 && y-2 >= 0){
                allowedXY[xySize][0] = x+1;
                allowedXY[xySize][1] = y-2;
                xySize += 1;
            }
            if(x-1<8 && y-2<8 && x-1 >= 0 && y-2 >= 0){
                allowedXY[xySize][0] = x-1;
                allowedXY[xySize][1] = y-2;
                xySize += 1;
            }
            if(x+2<8 && y+1<8 && x+2 >= 0 && y+1 >= 0){
                allowedXY[xySize][0] = x+2;
                allowedXY[xySize][1] = y+1;
                xySize += 1;
            }
            if(x+2<8 && y-1<8 && x+2 >= 0 && y-1 >= 0){
                allowedXY[xySize][0] = x+2;
                allowedXY[xySize][1] = y-1;
                xySize += 1;
            }
            if(x-2<8 && y+1<8 && x-2 >= 0 && y+1 >= 0){
                allowedXY[xySize][0] = x-2;
                allowedXY[xySize][1] = y+1;
                xySize += 1;
            }
            if(x-2<8 && y-1<8 && x-2 >= 0 && y-1 >= 0){
                allowedXY[xySize][0] = x-2;
                allowedXY[xySize][1] = y-1;
                xySize += 1;
            }
            break;
        case 'b':
                //  Top->bottom L->R : x+i,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && y+i < 8 && x+i >= 0 && y+i >= 0){
                        if(destPieceCol == pieceCol){
                            break;
                        }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //  Top->Bottom R->L : x-i,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && y+i < 8 && x-i >= 0 && y+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                // Bottom->Top R->L  : x+i,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && y-i < 8 && x+i >= 0 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Bottom->Top L->R   : x-i,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && y-i < 8 && x-i >= 0 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                break;
        case 'r':
                //Down : x,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(y+i < 8 && y+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Up : x,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(y-i < 8 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Left->Right : x+i,y
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && x+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Right->Left : x-i,y
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && x-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        break;
                      }
                    }
                }
            break;
        case 'q':
                //  Top->bottom L->R : x+i,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && y+i < 8 && x+i >= 0 && y+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //  Top->Bottom R->L : x-i,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && y+i < 8 && x-i >= 0 && y+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                // Bottom->Top R->L  : x+i,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && y-i < 8 && x+i >= 0 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Bottom->Top L->R   : x-i,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && y-i < 8 && x-i >= 0 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Top->Bottom : x,y+i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x,y+i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(y+i < 8 && y+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y+i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Bottom->Top : x,y-i
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x,y-i)].pieceColor;
                    //Friendly piece in destination:  break
                    if(y-i < 8 && y-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x;
                        allowedXY[xySize][1] = y-i;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Left->Right : x+i,y
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x+i,y)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x+i < 8 && x+i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x+i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        break;
                      }
                    }
                }
                //Right->Left : x-i,y
                for(int i = 1; i < 8; i += 1){
                    int destPieceCol = positions[getPos(x-i,y)].pieceColor;
                    //Friendly piece in destination:  break
                    if(x-i < 8 && x-i >= 0){
                    if(destPieceCol == pieceCol){
                        break;
                    }
                    //empty cell:  add and continue
                    if(destPieceCol == 2){
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        continue;
                    }
                    //enemy cell : add and break
                    else{
                        allowedXY[xySize][0] = x-i;
                        allowedXY[xySize][1] = y;
                        xySize += 1;
                        break;
                      }
                    }
                }
            break;
        case 'k':
            xySize = 0;
            //Right
            if(x+1<8 && x+1 >= 0){
                allowedXY[xySize][0] = x+1;
                allowedXY[xySize][1] = y;
                xySize += 1;
            }
            //Left
            if(x-1<8 && x-1 >= 0){
                allowedXY[xySize][0] = x-1;
                allowedXY[xySize][1] = y;
                xySize += 1;
            }
            //Up
            if(y-1<8 && y-1 >= 0){
                allowedXY[xySize][0] = x;
                allowedXY[xySize][1] = y-1;
                xySize += 1;
            }
            //Down
            if(y+1<8 && y+1>= 0){
                allowedXY[xySize][0] = x;
                allowedXY[xySize][1] = y+1;
                xySize += 1;
            }
            //Top Right
            if(x+1<8 && y-1<8 && x+1 >= 0 && y-1 >= 0){
                allowedXY[xySize][0] = x+1;
                allowedXY[xySize][1] = y-1;
                xySize += 1;
            }
            //Top Left
            if(x-1<8 && y-1<8 && x-1>= 0 && y-1 >= 0){
                allowedXY[xySize][0] = x-1;
                allowedXY[xySize][1] = y-1;
                xySize += 1;
            }
            //Bottom Right
            if(x+1<8 && y+1<8 && x+1 >= 0 && y+1 >= 0){
                allowedXY[xySize][0] = x+1;
                allowedXY[xySize][1] = y+1;
                xySize += 1;
            }
            //Bottom Left
            if(x-1<8 && y+1<8 && x-1>= 0 && y+1 >= 0){
                allowedXY[xySize][0] = x-1;
                allowedXY[xySize][1] = y+1;
                xySize += 1;
            }
            break;
    }

    for(int i = 0; i < xySize; i++){
        int tempPos = getPos(allowedXY[i][0],allowedXY[i][1]);
        GtkWidget *widget;
        if(!(tempPos > 63 || tempPos < 0) && positions[tempPos].pieceColor != pieceCol){
                allowedPos[allowedPosSize] = tempPos;
                allowedPosSize += 1;
                //highlight the path if flag == 1
                if(flag == 1){
                    if(positions[tempPos].pieceName == '\0'){
                        widget = box("dot.png", positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,0);
                    }
                    else if(positions[tempPos].pieceColor != pieceCol){
                        widget = box(positions[tempPos].path, positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,2);
                    }else{
                        widget = box(positions[tempPos].path, positions[tempPos].pos, positions[tempPos].x, positions[tempPos].y, positions[tempPos].pieceColor,1);
                    }
                    gtk_grid_attach(GTK_GRID(grid), widget, 10 * (positions[tempPos].x), 10 * (positions[tempPos].y), 10, 10);
                    gtk_widget_set_hexpand(widget, TRUE);
                    gtk_widget_set_vexpand(widget, TRUE);
                }
                //else only check for check and show dialog
                else{
                    char *title;
                    char *message;
                    if(positions[tempPos].pieceName == 'k'){
                        if(pieceCol){
                            g_print("Check");
                            title = "White Says: ";
                        }
                        else{
                            title = "Black Says: ";
                        }
                        message = "Check";
                        showDialog(widget,window,title,message);
                }
              }
        }
        gtk_widget_show_all(window);
    }
}

int checkInArray(int pos){
    for(int i = 0; i < allowedPosSize; i++){
        if(pos == allowedPos[i]){
            return 1;
            break;
        }
    }
    return 0;
}

int getPos(int i, int j){
    return(8*j + i);
}
