#include<bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_mixer.h>
//#include<SDL_ttf.h>
using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());
#define rand rd;

int Rand(int l, int r) { // sinh 1 số ngẫu nhiên trong đoạn [l; r]
    return l+(1ll*rd()*rd()%(r-l+1) +(r-l+1))%(r-l+1);
}


int frame=0, cntcharacter=0;
const int delays=6;

bool init();
bool loadMedia();
void close();

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class LTexture
{
	public:
	    int x=0;
	    int y=0;
		LTexture();
		~LTexture();
		bool loadFromFile( string path );
		void free();
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		void setBlendMode( SDL_BlendMode blending );
		void setAlpha( Uint8 alpha );
		void render( int x, int y, SDL_Rect* clip = NULL, bool flip=false, bool hero=false);
		int getWidth();
		int getHeight();

	private:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

LTexture gSpriteSheetTexture;

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 255, 255, 255 ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

void LTexture::render( int x, int y, SDL_Rect* clip, bool flip, bool hero){
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	if(!flip) SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
	else {
	    SDL_RendererFlip flipType= SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, 0.0, nullptr, flipType);
	}
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Maria and the mysterious treasure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

void close()
{
	//Free loaded images
	gSpriteSheetTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int w[] =           {-1,  48,  64,  48,  64,  128, 11};
int h[] =           {-1,  96,  64,  48,  64,  64,  11};
int HPs[] =         {-1,  3,   1,   10,  2,   7,   2};
int atks[] =        {-1,  1,   1,   1,   1,   1,   0};
int speeds[] =      {-1,  3,   2,   1,   0,   2,   0};
int CanAttacks[] =  {-1,  1,   0,   0,   1,   1,   5};

struct Character{
    LTexture Text;
    int id;
    int thutu;
    int x;
    int y;
    int u;
    int v;
    int HP;
    int atk;
    int speed;
    bool huong;
    bool jump;
    int jumpframe;
    int dx=0;
    int dy=0;
    int idleframe;
    bool CanAttack;
    int deathframe=0;

    Character() {
        id=0;
        thutu=0;
        x=0;
        y=0;
        Text.x = x;
        Text.y = y;
        u=0;
        v=0;
        HP=0;
        atk=0;
        speed=0;
        huong=0;
        jump=0;
        jumpframe=0;
        dx=0;
        dy=0;
        idleframe=0;
        CanAttack=false;
        deathframe=0;
    }

    Character(int idd, int stt, int xx, int yy, int ww=0, int hh=0) {
        id = idd;
        thutu = stt;
        x = xx;
        y = yy;
        Text.x = x;
        Text.y = y;
        u = x+w[idd];
        v = y+h[idd];
        if(ww!=0) u = x+ww;
        if(hh!=0) v = y+hh;
        HP = HPs[idd];
        atk = atks[idd];
        speed = speeds[idd];
        if(idd!=1) huong=1;
        else huong=0;
        jump=0;
        jumpframe=0;
        dx=0;
        dy=0;
        idleframe=0;
        CanAttack=CanAttacks[idd];
        deathframe=0;
    }
};

vector<Character> listnhanvat;

void CreateCharacter(int id, int x_start, int y_start, int w=0, int h=0) { //tạo object
    Character aaa(id, ++cntcharacter, x_start, y_start, w, h);
    listnhanvat.push_back(aaa);
}

void ClearCharacter(int thutu) { //xóa object
    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu == thutu) {
            listnhanvat.erase(listnhanvat.begin() + i);
        }
}

string NumToString(int x) {
    string res="";
    if(x==0) return "0";
    while(x>0) {
        char c='0'+x%10;
        res=c+res;
        x/=10;
    }
    return res;
}

string taolinkcharacter(int id, string thaotac) { //tạo link ảnh nhân vật
    string res="image/"; //sửa lại theo tên folder của u
    res+=NumToString(id);
    res+='/';
    res+=thaotac;
    res+=".png";
    return res;
}

void LoadSpriteCharacter(LTexture &Textt, int id, string thaotac, int frame, int timedelay, bool flip, int w=0, int h=0) { //load hoạt ảnh object
    string path = taolinkcharacter(id, thaotac);
    if(!Textt.loadFromFile(path.c_str())) {
        cout<<"Khong mo duoc anh sau: "<<path<<"\n";
        exit(0);
    }

    int xsum=Textt.getWidth();
    int ysum=Textt.getHeight();
    //chỉ cần truyền tham số cho w và h nếu là "pipe"
    if(w!=0) xsum=w;
    if(h!=0) ysum=h;

    int numsheets = xsum/ysum;
    if(id==1) numsheets=(xsum*2)/ysum;
    if(id==5) numsheets/=2;
    if(id==1 && thaotac=="Special") numsheets=9;
    if(id==1 && thaotac=="Idle") numsheets=1;
    if(id==6) numsheets=1;

    SDL_Rect KichThuoc[numsheets+1];
    for(int i=0; i<numsheets; i++) {
        KichThuoc[i].x= i*(xsum/numsheets);
        KichThuoc[i].y= 0;
        KichThuoc[i].w= xsum/numsheets;
        KichThuoc[i].h= ysum;
    }

    SDL_Rect* currentClip = &KichThuoc[ (frame / timedelay)%numsheets ];
    Textt.render( Textt.x , Textt.y, currentClip, flip );
}

string taolinkobject(int id) { //tạo link ảnh vật phẩm
    string res="assets/";
    res+=NumToString(id);
    res+=".png";
    return res;
}

void LoadSpriteObject(LTexture &Textt, int id, int frame, int timedelay) { //load hoạt ảnh object
    string path = taolinkobject(id);
    if(!Textt.loadFromFile(path.c_str())) {
        cout<<"Khong mo duoc anh sau: "<<path<<"\n";
        exit(0);
    }

    int xsum=Textt.getWidth();
    int ysum=Textt.getHeight();

    int numsheets = 1;
    if(id==9) numsheets=4;
    if(id==6 || id==7 || id==8) numsheets=5;
    if(id==3) numsheets=6;

    SDL_Rect KichThuoc[numsheets+1];
    for(int i=0; i<numsheets; i++) {
        KichThuoc[i].x= i*(xsum/numsheets);
        KichThuoc[i].y= 0;
        KichThuoc[i].w= xsum/numsheets;
        KichThuoc[i].h= ysum;
    }

    SDL_Rect* currentClip = &KichThuoc[ (frame / timedelay)%numsheets ];
    Textt.render( Textt.x , Textt.y, currentClip, false);
}

bool GiaoNhau(int l, int r, int u, int v) {
    return (max(l, u) < min(r, v));
}

int KhoangCach(int l, int r, int u, int v) {
    return abs( max(l, u) - min(r, v) );
}

void HoatDongMVy(Character &ndmaivy) {//chạy nhân vật chính
    //thêm máu <=0 -> thua
    string thaotac="Idle";

    ndmaivy.idleframe++;
    int deltax=ndmaivy.dx, deltay=6;

///~~~~~~xử lý di chuyển theo phương x~~~~~~~~~~~~

    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu != ndmaivy.thutu) {

            if( ndmaivy.v <= listnhanvat[i].y ) continue;

            if(GiaoNhau(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v))
                if(GiaoNhau(ndmaivy.x+deltax, ndmaivy.u+deltax, listnhanvat[i].x, listnhanvat[i].u)) {
                    if(!ndmaivy.huong) deltax=min(deltax, KhoangCach(ndmaivy.x, ndmaivy.u, listnhanvat[i].x, listnhanvat[i].u));
                    else deltax=max(deltax, -KhoangCach(ndmaivy.x, ndmaivy.u, listnhanvat[i].x, listnhanvat[i].u));
                    break;
                }
            }
    if(deltax!=0) ndmaivy.idleframe=0;

///~~~~~~xử lý di chuyển theo phương y~~~~~~~~~~~~
    bool NormalY=true;
    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu != ndmaivy.thutu) {
            //tính toán deltay ban đầu
            int deltayao;
            if(ndmaivy.jump || ndmaivy.jumpframe!=0) { //nếu đang trong status jump
                if(ndmaivy.jumpframe < 12) deltayao = -6;
                else if(ndmaivy.jumpframe < 24) deltayao = -3;
                else if(ndmaivy.jumpframe < 36) deltayao = 3;
                else deltayao=6;
            }

            //ktra xem nếu rơi xuống thì có bị lố không
            if(GiaoNhau(ndmaivy.x+deltax, ndmaivy.u+deltax, listnhanvat[i].x, listnhanvat[i].u))
                if(ndmaivy.v+deltayao >= listnhanvat[i].y) {
                    NormalY=false;
                    ndmaivy.jumpframe=0;
                    deltay= min(deltay, KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v) );

                    if(KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v)!=0 && listnhanvat[i].id<=5) listnhanvat[i].HP-=5; //gây sát thương cho địch khi đang rơi

                    break;
                }
        }

    bool OnObject=false;

    //(Tạm) TH đang giẫm lên nền
    if(ndmaivy.v==496) {OnObject=true; deltay=0;}

    //Kiểm tra TH đang giẫm lên trên con khác
    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu != ndmaivy.thutu)
            if(GiaoNhau(ndmaivy.x+deltax, ndmaivy.u+deltax, listnhanvat[i].x, listnhanvat[i].u))
                if(KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v) == 0) {
                    OnObject=true;
                    ndmaivy.jumpframe=0;
                    deltay=0;
                    break;
                }

    //Trường hợp bình thường, bên trên là các trường hợp đặc biệt
    if(NormalY && ( (ndmaivy.jump && OnObject) || ndmaivy.jumpframe!=0)) {
        ndmaivy.idleframe=0;

        if(ndmaivy.jumpframe < 12) deltay = -6;
        else if(ndmaivy.jumpframe < 24) deltay = -3;
        else if(ndmaivy.jumpframe < 36) deltay = 3;
        else deltay=6;

        ndmaivy.jumpframe++;
        if(ndmaivy.jumpframe == 48) {
            ndmaivy.jumpframe = 0;
        }
    }
    else if(!OnObject) {
        deltay=min(min(deltay, 6), abs(ndmaivy.v - 496) );
        //ktra xem rơi theo trọng lực thì có bị lố không
        for(int i=0; i<listnhanvat.size(); i++)
            if(listnhanvat[i].thutu != ndmaivy.thutu)
                if(GiaoNhau(ndmaivy.x+deltax, ndmaivy.u+deltax, listnhanvat[i].x, listnhanvat[i].u))
                    if(ndmaivy.v+deltay >= listnhanvat[i].y) {
                        NormalY=false;
                        ndmaivy.jumpframe=0;
                        deltay= min(deltay, KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v) );

                        if(KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v)!=0 && listnhanvat[i].id<=5) listnhanvat[i].HP-=5; //gây sát thương cho địch khi đang rơi

                        break;
                    }

    }


///~~~~~~~~~~ghi nhận kết quả~~~~~~~~~~~~~
    if(deltax!=0) thaotac="Walk";
    if(deltay!=0) thaotac="Jump";

    ndmaivy.x += deltax;
    ndmaivy.u += deltax;
    ndmaivy.Text.x += deltax;

    ndmaivy.y += deltay;
    ndmaivy.v += deltay;
    ndmaivy.Text.y += deltay;

    if(ndmaivy.idleframe>=336 && ndmaivy.id==1) thaotac="Spin";

///~~~~~~~rendering~~~~~~~~~~~~~~~~~

    if(thaotac=="Jump") LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, ndmaivy.jumpframe, delays, ndmaivy.huong);
    else LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, frame, delays, ndmaivy.huong);

    LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, "Heart", frame, delays, ndmaivy.huong);

    //show HP of main character
    LTexture heartt;
    for(int i=1; i<=ndmaivy.HP; i++) {
        heartt.x=(i-1)*40+10;
        heartt.y=10;
        LoadSpriteObject(heartt, 1, frame, delays);
    }
}

void HoatDong(Character &doituong) {//chạy object phụ
    if(doituong.id>=6) {
        LoadSpriteCharacter(doituong.Text, 6, "pipe", frame, delays, 0, doituong.u-doituong.x, doituong.v-doituong.y);
        return;
    }

    if(doituong.HP<=0) {
        int huongreal=doituong.huong;
        if(doituong.id!=1) huongreal = 1-huongreal;

        LoadSpriteCharacter(doituong.Text, doituong.id, "Death", doituong.deathframe, delays, huongreal);
        doituong.deathframe++;
        if(doituong.deathframe==48) ClearCharacter(doituong.thutu);
        return;
    }
    //thêm bắn đạn (Create Object + reloadframe)
    string thaotac="Idle";

    int deltax=doituong.speed;

    if(doituong.huong) deltax= 0-deltax;
    bool DoiChieu=false;

    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu != doituong.thutu)
            if(GiaoNhau(doituong.y, doituong.v, listnhanvat[i].y, listnhanvat[i].v))
                if(GiaoNhau(doituong.x+deltax, doituong.u+deltax, listnhanvat[i].x, listnhanvat[i].u)) {
                    //va nhau -> thay đổi deltax để không bị đi lố + đổi chiều nếu cần
                    deltax= min( abs(deltax), KhoangCach(doituong.x, doituong.u, listnhanvat[i].x, listnhanvat[i].u) );
                    if(doituong.huong) deltax = 0 - deltax;
                    if(deltax == 0) {
                        DoiChieu=true;
                        doituong.huong = 1 - doituong.huong;
                    }

                    //kiểm tra nếu đối tượng kia là nvchinh -> trừ máu nhân vật chính
                    if(deltax==0 && listnhanvat[i].id==1) listnhanvat[i].HP--;

                    break;
                }

    if(DoiChieu) { //ktra xem có bị kẹp 2 đầu k -> đứng im
        for(int i=0; i<listnhanvat.size(); i++)
            if(listnhanvat[i].thutu != doituong.thutu)
                if(GiaoNhau(doituong.y, doituong.v, listnhanvat[i].y, listnhanvat[i].v))
                    if(KhoangCach(doituong.x, doituong.u, listnhanvat[i].x, listnhanvat[i].u)==0)
                        if( (!doituong.huong && doituong.x<listnhanvat[i].x) || (doituong.huong && doituong.x>listnhanvat[i].x) ){
                            doituong.huong = 1 - doituong.huong;
                            break;
                        }
    }

    if(deltax!=0) thaotac="Walk";

    doituong.x += deltax;
    doituong.u += deltax;
    doituong.Text.x += deltax;

    int huongreal=doituong.huong;
    if(doituong.id!=1) huongreal = 1-huongreal;
    LoadSpriteCharacter(doituong.Text, doituong.id, thaotac, frame, delays, huongreal);
}

int main( int argc, char* args[] ){
    srand(time(NULL));
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		return 0;
	}

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

	//xóa màn hình về màn hình trắng
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
    SDL_RenderClear( gRenderer );


    CreateCharacter(1, 100, 400);
    CreateCharacter(2, 400, 432);
    CreateCharacter(3, 600, 448);
    CreateCharacter(4, 800, 432);
    CreateCharacter(5, 1000, 432);
    CreateCharacter(6, 300, 426, 50, 70);

    //While application is running
    while( !quit ) //vòng lặp chính của game
    {
        //Handle events on queue
        Character *ndmaivy;
        for(int i=0; i<listnhanvat.size(); i++)
            if(listnhanvat[i].id==1) ndmaivy = &listnhanvat[i];
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            else if( e.type == SDL_KEYDOWN ){
                if(e.key.keysym.sym == SDLK_d) {
                    ndmaivy->dx=3;
                    ndmaivy->huong=false;
                    continue;
                }
                if(e.key.keysym.sym == SDLK_a) {
                    ndmaivy->dx=-3;
                    ndmaivy->huong=true;
                    continue;
                }
                if(e.key.keysym.sym == SDLK_w) {
                    ndmaivy->jump=true;
                    continue;
                }
            }
            else if( e.type == SDL_KEYUP ){
                if(e.key.keysym.sym == SDLK_d) {
                    ndmaivy->dx=0;
                    continue;
                }
                if(e.key.keysym.sym == SDLK_a) {
                    ndmaivy->dx=0;
                    continue;
                }
                if(e.key.keysym.sym == SDLK_w) {
                    ndmaivy->jump=false;
                    continue;
                }
            }
        }

        SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
        SDL_RenderClear( gRenderer );

        for(int i=0; i<listnhanvat.size(); i++)
            if(listnhanvat[i].id==1) HoatDongMVy(listnhanvat[i]);
            else HoatDong(listnhanvat[i]);

//        LTexture cot1;
//        cot1.x=100;
//        cot1.y=100;
//        LoadSpriteObject(cot1, "pipe", frame, delays, 50, 200);
//
//        LTexture cot2;
//        cot2.x=200;
//        cot2.y=100;
//        LoadSpriteObject(cot2, "pipe", frame, delays, 50, 100);
//        LTexture cot3;
//        cot3.x=300;
//        cot3.y=436;
//        LoadSpriteCharacter(cot3, 6, "pipe", frame, delays, 0, 50, 60);


        SDL_RenderPresent( gRenderer );
        frame++;
    }
    //Mai làm nốt mấy hàm show địch, spawn địch, di chuyển nhân vật chính-địch, tương tác nv chính-địch

	//Free resources and close SDL
	close();

	return 0;
}
