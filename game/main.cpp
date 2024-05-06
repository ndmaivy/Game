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


int trangthai=0, frame=0, cntcharacter=0, score=0, maxHP=3, f[4*3005], sunframe=-1, xsun=-1, ysun=-1;
bool chedokho=false;
const int delays=6;

bool init();
void loadMedia();
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
		void render( int x, int y, SDL_Rect* clip, bool flip);
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

void LTexture::render( int x=0, int y=0, SDL_Rect* clip=nullptr, bool flip=false){
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

class LButton
{
	public:
	    int x, y, w, h;
		LButton();
		LButton(int xx, int yy, int ww, int hh);
		void setPosition( int x, int y );
		bool MouseClick( SDL_Event *e );

//	private:
//		SDL_Point mPosition;
//		SDL_Point Size;
};

LButton::LButton()
{
	x=0;
	y=0;
	w=0;
	h=0;
}

LButton::LButton(int xx, int yy, int ww, int hh)
{
	x=xx;
	y=yy;
	w=ww;
	h=hh;
}

void LButton::setPosition( int xx, int yy )
{
	x = xx;
	y = yy;
}

bool LButton::MouseClick( SDL_Event* e )
{
	if(e->type == SDL_MOUSEBUTTONDOWN)
	{
		//Get mouse position
		int x_mouse, y_mouse;
		SDL_GetMouseState( &x_mouse, &y_mouse );

		bool inside = true;

		if( x_mouse < x ) inside = false;
		else if( x_mouse > x + w ) inside = false;
		else if( y_mouse < y ) inside = false;
		else if( y_mouse > y + h ) inside = false;


		if(inside){
			if( e->type == SDL_MOUSEBUTTONDOWN)
				return true;
		}
	}
	return false;
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

void updatesegment(int id, int l, int r, int x, int val) {
    if(r<x || x<l) return;
    if(l==r) {
        f[id]+=val;
        return;
    }
    int mid=(l+r)/2;
    updatesegment(id*2, l, mid, x, val);
    updatesegment(id*2+1, mid+1, r, x, val);
    f[id]=f[id*2]+f[id*2+1];
}

int getsegment(int id, int l, int r, int x, int y) {
    if(r<x || y<l) return 0;
    if(x<=l && r<=y) return f[id];
    int mid=(l+r)/2;
    return getsegment(id*2,l,mid,x,y)+getsegment(id*2+1,mid+1,r,x,y);
}

///dữ liệu characters
int w[] =           {-1,  48,  64,  48,  64,  128, 12};
int h[] =           {-1,  96,  64,  48,  64,  64,  7};
int HPs[] =         {-1,  3,   1,   10,  2,   7,   2};
int atks[] =        {-1,  1,   1,   1,   1,   1,   0};
int speeds[] =      {-1,  3,   2,   1,   0,   2,   0};
int CanAttacks[] =  {-1,  1,   0,   0,   1,   1,   5};
///dữ liệu objects (không cần mảng atk dành riêng cho đạn)
int w2[] =          {-1,  30,  20,  16,  10,  16,  16,  16,  32,  20,  17,  25,  30};
int h2[] =          {-1,  30,  25,  17,  20,  16,  16,  16,  20,  20,  17,  15,  30};
int speed2[] =      {-1,  0,   0,   0,   0,   0,   0,   0,   0,   7,   7,   7,   0};

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
    int dx;
    int dy;
    int idleframe;
    bool CanAttack;
    int deathframe;
    bool buff;
    int buffframe;
    int ammo;
    bool attack;
    int attackframe;
    int reloadframe;

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
        buff=false;
        buffframe=0;
        ammo=5;
        attack=false;
        attackframe=0;
        reloadframe=0;
    }

    Character(int idd, int stt, int xx, int yy, int ww=0, int hh=0, bool vatpham=false, bool huongg=false) {
        id = idd;
        thutu = stt;

        x = xx;
        y = yy;
        Text.x = x;
        Text.y = y;

        if(!vatpham) {
            u = x+w[idd];
            v = y+h[idd];
            if(ww!=0) u = x+ww;
            if(hh!=0) v = y+hh;

            if(idd==1) {
                y+=20;
                u-=15;
            }
            if(idd==2) y+=25;
            if(idd==4) y+=12;
            if(idd==5) {
                y+=12;
                x+=35;
                u-=15;
            }

            HP = HPs[idd];
            atk = atks[idd];
            speed = speeds[idd];
            if(idd!=1) huong=1;
            else huong=0;
        }
        else {
            u = x+w2[idd];
            v = y+h2[idd];
            HP = 1;
            atk = 0;
            if(9<=idd && idd<=11) atk=1;
            speed = speed2[idd];
            huong=huongg;
        }

        jump=0;
        jumpframe=0;
        dx=0;
        dy=0;
        idleframe=0;
        CanAttack=CanAttacks[idd];
        deathframe=0;
        buff=false;
        buffframe=0;
        ammo=5;
        attack=false;
        attackframe=0;
        reloadframe=0;
    }
};

vector<Character> listnhanvat, listvatpham;

///Điểm khác giữa loại Character và loại Object:
///Charecter có hit box xung quanh, có tương tác di chuyển. Ví dụ: nhân vật chính, quái, vật cản(map, cột, ...). Hình ảnh của chúng được lưu trong folder characters
///Object không có hitbox và tương tác di chuyển. Hình ảnh của chúng được lưu trong folder objects

void CreateCharacter(int id, int x_start, int y_start, int w=0, int h=0) { //tạo character
    Character aaa(id, ++cntcharacter, x_start, y_start, w, h);
    if(id==1) aaa.HP = min(aaa.HP, maxHP);
    if(id!=1) updatesegment(1, 1, 3000, (aaa.x+aaa.u)/2, 1);
    listnhanvat.push_back(aaa);
}

void ClearCharacter(int thutu) { //xóa character
    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu == thutu) {
            listnhanvat.erase(listnhanvat.begin() + i);
        }
}

void CreateObject(int id, int x_start, int y_start, int w=0, int h=0, bool huongg=false) {
    Character aaa(id, ++cntcharacter, x_start, y_start, w, h, true, huongg);
    listvatpham.push_back(aaa);
}

void ClearObject(int thutu) { //xóa character
    for(int i=0; i<listvatpham.size(); i++)
        if(listvatpham[i].thutu == thutu) {
            listvatpham.erase(listvatpham.begin() + i);
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
    string res="characters/"; //sửa lại theo tên folder của u
    res+=NumToString(id);
    res+='/';
    res+=thaotac;
    res+=".png";
    return res;
}

string taolinkobject(int id) { //tạo link ảnh vật phẩm
    string res="objects/";
    res+=NumToString(id);
    res+=".png";
    return res;
}

string taolinkmenu(string namee) { //tạo link ảnh vật phẩm
    string res="menu/";
    res+=namee;
    res+=".png";
    return res;
}

void LoadSpriteCharacter(LTexture &Textt, int id, string thaotac, int frame, int timedelay, bool flip, int w=0, int h=0) { //load hoạt ảnh character
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
    if(id==7) numsheets=6;

    SDL_Rect KichThuoc[numsheets+1];
    for(int i=0; i<numsheets; i++) {
        KichThuoc[i].x= i*(xsum/numsheets);
        KichThuoc[i].y= 0;
        KichThuoc[i].w= xsum/numsheets;
        KichThuoc[i].h= ysum;
    }

    SDL_Rect* currentClip = &KichThuoc[ (frame / timedelay)%numsheets ];
    if(thaotac!="Heart") Textt.render( Textt.x , Textt.y, currentClip, flip );
    else Textt.render( Textt.x , Textt.y+20, currentClip, flip );
}

void LoadSpriteObject(LTexture &Textt, int id, int frame, int timedelay, bool flip=false) { //load hoạt ảnh object
    string path = taolinkobject(id);
    if(!Textt.loadFromFile(path.c_str())) {
        cout<<"Khong mo duoc anh sau: "<<path<<"\n";
        exit(0);
    }

    int xsum=Textt.getWidth();
    int ysum=Textt.getHeight();

    int numsheets = 1;
    if(id==8) numsheets=4;
    if(id==5 || id==6 || id==7) numsheets=5;
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

void HoatDongVatPham(Character &ndmaivy) { //Cho vật phẩm bay + vẽ vật phẩm + nhặt vật phẩm
    for(int i=0; i<listvatpham.size(); i++) {
        int deltax=listvatpham[i].speed;
        if(listvatpham[i].huong) deltax *= -1;
        listvatpham[i].x += deltax;
        listvatpham[i].u += deltax;
        listvatpham[i].Text.x += deltax;

        LoadSpriteObject(listvatpham[i].Text, listvatpham[i].id, frame, delays, listvatpham[i].huong);

        if(GiaoNhau(listvatpham[i].x, listvatpham[i].u, ndmaivy.x, ndmaivy.u))
            if(GiaoNhau(listvatpham[i].y, listvatpham[i].v, ndmaivy.y, ndmaivy.v)) {

                if(listvatpham[i].id == 2) ndmaivy.ammo = min(5, ndmaivy.ammo+2);
                if(listvatpham[i].id == 3) ndmaivy.HP = min(maxHP, ndmaivy.HP+1);
                if(listvatpham[i].id == 4) {ndmaivy.buff=true; ndmaivy.buffframe=0;}
                if(listvatpham[i].id == 5) score+=10;
                if(listvatpham[i].id == 6) score+=100;
                if(listvatpham[i].id == 7) score+=1000;
                if(9 <= listvatpham[i].id && listvatpham[i].id<=11) ndmaivy.HP--;
                //if(listvatpham[i].id == 12) show màn hình cuối cùng

                ClearObject(listvatpham[i].thutu);
            }
    }
}

void HoatDongVatPham2(Character &doituong) { //kiểm tra quái/vật cản trúng đạn
    for(int i=0; i<listvatpham.size(); i++)
        if(9 <= listvatpham[i].id && listvatpham[i].id<=11)
            if(GiaoNhau(listvatpham[i].x, listvatpham[i].u, doituong.x, doituong.u))
                if(GiaoNhau(listvatpham[i].y, listvatpham[i].v, doituong.y, doituong.v)) {
                    doituong.HP--;

                    if(listvatpham[i].id == 9)
                        for(int i=0; i<listnhanvat.size(); i++)
                            if(listnhanvat[i].id==1 && listnhanvat[i].buff) {
                                doituong.HP--;
                                break;
                            }

                    ClearObject(listvatpham[i].thutu);
                }
}

void RoiVatPham(Character &doituong) {
    int chance=Rand(1, 100);
    int newx, newy, id=0;
    if(1<=chance && chance<=15) id=2;
    if(16<=chance && chance<=25) id=3;
    if(26<=chance && chance<=30) id=4;
    if(31<=chance && chance<=70) id=5;
    if(71<=chance && chance<=80) id=6;
    if(81<=chance && chance<=82) id=7;

    if(chedokho) {
        id=0;
        if(1<=chance && chance<=10) id=2;
        if(11<=chance && chance<=20) id=4;
        if(21<=chance && chance<=45) id=5;
        if(46<=chance && chance<=50) id=6;
        if(chance==51) id=7;
    }

    if(id!=0) {
        newx= doituong.x + ((doituong.u-doituong.x)-w2[id] )/2;
        newy= doituong.y + ((doituong.v-doituong.y)-h2[id] )/2;
        CreateObject(id, newx, newy);
    }
}

void HoatDongMVy(Character &ndmaivy) {//chạy nhân vật chính
    //thêm máu <=0 -> thua
    if(ndmaivy.HP <= 0) {
        cout<<"YOU LOSED!";
        exit(0);
    }

    string thaotac="Idle";

    ndmaivy.idleframe++;
    int deltax=ndmaivy.dx, deltay=6;
    int doituongbigiam=-1;

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

                    if(KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v)!=0 && listnhanvat[i].id<=5) doituongbigiam=i; //gây sát thương cho địch khi đang rơi (giẫm)

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

                        if(KhoangCach(ndmaivy.y, ndmaivy.v, listnhanvat[i].y, listnhanvat[i].v)!=0 && listnhanvat[i].id<=5) doituongbigiam=i; //gây sát thương cho địch khi đang rơi

                        break;
                    }

    }


///~~~~~~~~~~ghi nhận kết quả~~~~~~~~~~~~~
    if(deltax!=0) thaotac="Walk";
    if(deltay!=0) thaotac="Jump";

    if(ndmaivy.buff) {
        ndmaivy.buffframe++;
        if(ndmaivy.buffframe==1111) {
            ndmaivy.buff=false;
            ndmaivy.buffframe=0;
        }
    }

    ndmaivy.x += deltax;
    ndmaivy.u += deltax;
    ndmaivy.Text.x += deltax;

    ndmaivy.y += deltay;
    ndmaivy.v += deltay;
    ndmaivy.Text.y += deltay;

    //trừ máu thằng bị nhảy lên
    if(doituongbigiam!=-1) listnhanvat[ doituongbigiam ].HP -= 5;
    if(listnhanvat[ doituongbigiam ].id==3 && listnhanvat[ doituongbigiam ].HP<=5 && listnhanvat[ doituongbigiam ].HP>0) { //do snail sang phase 2 sẽ bị lố hình ảnh nên phải giảm bớt
        ndmaivy.y += 8;
        ndmaivy.v += 8;
        ndmaivy.Text.y += 8;
        listnhanvat[ doituongbigiam ].y += 8;
    }

    //bắn bắn
    if(ndmaivy.attack && ndmaivy.reloadframe == 0 && ndmaivy.ammo>=1) {
        int newx, newy;
        newy = ((ndmaivy.v-ndmaivy.y) -h2[9])/2 + ndmaivy.y;
        if(ndmaivy.huong) newx = ndmaivy.x-w2[9];
        else newx= ndmaivy.u+1;

        CreateObject(9, newx, newy, 0, 0, ndmaivy.huong);
        ndmaivy.reloadframe=1;
        ndmaivy.ammo--;
    }
    if(ndmaivy.reloadframe!=0) ndmaivy.reloadframe++;
    if(ndmaivy.reloadframe==50) ndmaivy.reloadframe=0;
    if(frame%500==0) ndmaivy.ammo=min(ndmaivy.ammo+1, 5);

    if(ndmaivy.idleframe>=336 && ndmaivy.id==1) thaotac="Spin";

    HoatDongVatPham(ndmaivy);

///~~~~~~~rendering~~~~~~~~~~~~~~~~~

    if(thaotac=="Jump") LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, ndmaivy.jumpframe, delays, ndmaivy.huong);
    else LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, frame, delays, ndmaivy.huong);

    if(ndmaivy.buff) LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, "Heart", frame, delays, ndmaivy.huong);

    //show HP of main character
    LTexture fake;
    for(int i=1; i<=ndmaivy.HP; i++) {
        fake.x=(i-1)*40+10;
        fake.y=10;
        LoadSpriteObject(fake, 1, frame, delays);
    }
    //show ammo of main character
    for(int i=1; i<=ndmaivy.ammo; i++) {
        fake.x=(i-1)*20+10;
        fake.y=50;
        LoadSpriteObject(fake, 13, frame, delays);
    }
}

void HoatDong(Character &doituong) {//chạy object phụ
    HoatDongVatPham2(doituong);

    if(doituong.id>=6) {
        if(doituong.id == 6)LoadSpriteCharacter(doituong.Text, 6, "pipe", frame, delays, 0, doituong.u-doituong.x, doituong.v-doituong.y);
        return;
    }

    if(doituong.HP<=0) {
        int huongreal=doituong.huong;
        if(doituong.id!=1) huongreal = 1-huongreal;

        LoadSpriteCharacter(doituong.Text, doituong.id, "Death", doituong.deathframe, delays, huongreal);
        doituong.deathframe++;
        if(doituong.deathframe==4*delays) {
            RoiVatPham(doituong);
            updatesegment(1, 1, 3000, (doituong.x+doituong.u)/2, -1);
            ClearCharacter(doituong.thutu);
        }
        return;
    }
    //thêm bắn đạn (Create Object + reloadframe)
    string thaotac="Idle";

    int deltax=doituong.speed;

    if(doituong.id==3 && doituong.HP<=5) {
        deltax=0;
        thaotac="Shell";
    }

///~~~~~~~~~~~~~di chuyển~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

///~~~~~~~~~~~~~tấn công (bắn)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(doituong.CanAttack) {
        if(doituong.reloadframe==0) {
            deltax=0;
            thaotac="Attack";
            doituong.attackframe++;

            if(doituong.attackframe == 4*delays) {
                int id = doituong.id+6;
                int newx, newy;
                newy = ((doituong.v-doituong.y) - h2[id])/2 + doituong.y;
                if(doituong.huong) newx = doituong.x-w2[id];
                else newx= doituong.u+1;

                CreateObject(id, newx, newy, 0, 0, doituong.huong);
            }
            if(doituong.attackframe == 8*delays) {
                doituong.attackframe=0;
                doituong.reloadframe=1;
            }
        }
        if(doituong.reloadframe!=0) doituong.reloadframe++;
        if(chedokho && doituong.reloadframe==200) doituong.reloadframe=0;
        if(!chedokho && doituong.reloadframe==300) doituong.reloadframe=0;
    }

///~~~~~~~~~~~~ghi nhận kết quả~~~~~~~~~~~~~~~~~~~~~~~~~~~

    if(deltax!=0) thaotac="Walk";

    updatesegment(1, 1, 3000, (doituong.x+doituong.u)/2, -1);

    doituong.x += deltax;
    doituong.u += deltax;
    doituong.Text.x += deltax;

    updatesegment(1, 1, 3000, (doituong.x+doituong.u)/2, 1);

    int huongreal=doituong.huong;
    if(doituong.id!=1) huongreal = 1-huongreal;

    if(thaotac=="Attack") LoadSpriteCharacter(doituong.Text, doituong.id, thaotac, doituong.attackframe, delays, huongreal);
    else LoadSpriteCharacter(doituong.Text, doituong.id, thaotac, frame, delays, huongreal);
}

void UltiSun(Character *ndmaivy) {
    if(ndmaivy->ammo < 5) return;

    int bestt=-1, val=0, rangee=600;

    for(int i=0; i<listnhanvat.size(); i++)
        if(2<=listnhanvat[i].id && listnhanvat[i].id<=5) {
            int mid=(listnhanvat[i].x+listnhanvat[i].u)/2;
            int valuee=getsegment(1, 1, 3000, max(1, mid - rangee), mid);
            if(valuee>val) {
                val=valuee;
                bestt = i;
            }
        }

    if(bestt != -1) {
        int mid=(listnhanvat[bestt].x+listnhanvat[bestt].u)/2;
        ndmaivy->ammo -= 5;
        sunframe=0;

        int r=mid, l=mid;

        for(int i=0; i<listnhanvat.size(); i++)
        if(2<=listnhanvat[i].id && listnhanvat[i].id<=5) {
            int mid2=(listnhanvat[i].x+listnhanvat[i].u)/2;
            if(mid-rangee<= mid2 && mid2<= mid) {
                l=min(l, mid2);
                listnhanvat[i].HP=0;
            }
        }

        xsun=(l+r)/2 -25; //để tâm của mặt trời nằm giữa khoảng của những kẻ địch bị chọn bị trừng phạt
        ysun=listnhanvat[bestt].y-300; //để mặt trời trên cao
    }
}

void BuildMapStage1() {
    CreateCharacter(1, 100, 400);
    CreateCharacter(5, 400, 432);
    CreateCharacter(3, 600, 448);
    CreateCharacter(4, 800, 432);
    CreateCharacter(2, 1000, 432);
    CreateCharacter(6, 300, 421, 50, 75);
    CreateObject(2, 400, 450);
    CreateObject(3, 500, 450);
    CreateObject(4, 600, 450);
    CreateObject(5, 700, 450);
    CreateObject(6, 800, 450);
    CreateObject(7, 900, 450);
}

void LuuDuLieu(){
    ofstream outt("data.txt");

    outt<<trangthai<<" "<<frame<<" "<<cntcharacter<<" "<<score<<" "<<maxHP<<" "<<sunframe<<" "<<xsun<<" "<<ysun<<" "<<chedokho<<"\n";
    outt<<listnhanvat.size()<<"\n";
    for(int i=0; i<listnhanvat.size(); i++) {
        outt<<listnhanvat[i].id<<" ";
        outt<<listnhanvat[i].thutu<<" ";
        outt<<listnhanvat[i].x<<" ";
        outt<<listnhanvat[i].y<<" ";
        outt<<listnhanvat[i].Text.x<<" ";
        outt<<listnhanvat[i].Text.y<<" ";
        outt<<listnhanvat[i].u<<" ";
        outt<<listnhanvat[i].v<<" ";
        outt<<listnhanvat[i].HP<<" ";
        outt<<listnhanvat[i].atk<<" ";
        outt<<listnhanvat[i].speed<<" ";
        outt<<listnhanvat[i].huong<<" ";
        outt<<listnhanvat[i].jump<<" ";
        outt<<listnhanvat[i].jumpframe<<" ";
        outt<<listnhanvat[i].dx<<" ";
        outt<<listnhanvat[i].dy<<" ";
        outt<<listnhanvat[i].idleframe<<" ";
        outt<<listnhanvat[i].CanAttack<<" ";
        outt<<listnhanvat[i].deathframe<<" ";
        outt<<listnhanvat[i].buff<<" ";
        outt<<listnhanvat[i].buffframe<<" ";
        outt<<listnhanvat[i].ammo<<" ";
        outt<<listnhanvat[i].attack<<" ";
        outt<<listnhanvat[i].attackframe<<" ";
        outt<<listnhanvat[i].reloadframe<<" ";
        outt<<"\n";

        int mid=(listnhanvat[i].x + listnhanvat[i].u)/2;
        updatesegment(1, 1, 3000, mid, -1);
    }
    listnhanvat.clear();

    outt<<listvatpham.size()<<"\n";
    for(int i=0; i<listvatpham.size(); i++) {
        outt<<listvatpham[i].id<<" ";
        outt<<listvatpham[i].thutu<<" ";
        outt<<listvatpham[i].x<<" ";
        outt<<listvatpham[i].y<<" ";
        outt<<listvatpham[i].Text.x<<" ";
        outt<<listvatpham[i].Text.y<<" ";
        outt<<listvatpham[i].u<<" ";
        outt<<listvatpham[i].v<<" ";
        outt<<listvatpham[i].HP<<" ";
        outt<<listvatpham[i].atk<<" ";
        outt<<listvatpham[i].speed<<" ";
        outt<<listvatpham[i].huong<<" ";
        outt<<listvatpham[i].jump<<" ";
        outt<<listvatpham[i].jumpframe<<" ";
        outt<<listvatpham[i].dx<<" ";
        outt<<listvatpham[i].dy<<" ";
        outt<<listvatpham[i].idleframe<<" ";
        outt<<listvatpham[i].CanAttack<<" ";
        outt<<listvatpham[i].deathframe<<" ";
        outt<<listvatpham[i].buff<<" ";
        outt<<listvatpham[i].buffframe<<" ";
        outt<<listvatpham[i].ammo<<" ";
        outt<<listvatpham[i].attack<<" ";
        outt<<listvatpham[i].attackframe<<" ";
        outt<<listvatpham[i].reloadframe<<" ";
        outt<<"\n";
    }
    listvatpham.clear();

    outt.close();
}

void TaiDuLieu(){
    ifstream inpp("data.txt");

    int n, m;

    inpp>>trangthai >> frame >> cntcharacter >> score >> maxHP >> sunframe >> xsun >> ysun >> chedokho;
    inpp>>n;
    for(int i=0; i<n; i++) {
        Character aaa;
        inpp>>aaa.id;
        inpp>>aaa.thutu;
        inpp>>aaa.x;
        inpp>>aaa.y;
        inpp>>aaa.Text.x;
        inpp>>aaa.Text.y;
        inpp>>aaa.u;
        inpp>>aaa.v;
        inpp>>aaa.HP;
        inpp>>aaa.atk;
        inpp>>aaa.speed;
        inpp>>aaa.huong;
        inpp>>aaa.jump;
        inpp>>aaa.jumpframe;
        inpp>>aaa.dx;
        inpp>>aaa.dy;
        inpp>>aaa.idleframe;
        inpp>>aaa.CanAttack;
        inpp>>aaa.deathframe;
        inpp>>aaa.buff;
        inpp>>aaa.buffframe;
        inpp>>aaa.ammo;
        inpp>>aaa.attack;
        inpp>>aaa.attackframe;
        inpp>>aaa.reloadframe;

        listnhanvat.push_back(aaa);

        int mid=(aaa.x + aaa.u)/2;
        updatesegment(1, 1, 3000, mid, 1);
    }

    inpp>>m;
    for(int i=0; i<m; i++) {
        Character aaa;
        inpp>>aaa.id;
        inpp>>aaa.thutu;
        inpp>>aaa.x;
        inpp>>aaa.y;
        inpp>>aaa.Text.x;
        inpp>>aaa.Text.y;
        inpp>>aaa.u;
        inpp>>aaa.v;
        inpp>>aaa.HP;
        inpp>>aaa.atk;
        inpp>>aaa.speed;
        inpp>>aaa.huong;
        inpp>>aaa.jump;
        inpp>>aaa.jumpframe;
        inpp>>aaa.dx;
        inpp>>aaa.dy;
        inpp>>aaa.idleframe;
        inpp>>aaa.CanAttack;
        inpp>>aaa.deathframe;
        inpp>>aaa.buff;
        inpp>>aaa.buffframe;
        inpp>>aaa.ammo;
        inpp>>aaa.attack;
        inpp>>aaa.attackframe;
        inpp>>aaa.reloadframe;

        listvatpham.push_back(aaa);
    }

    inpp.close();
}

LTexture Textmainscr, Textmode, Textpause, Texthighscore, Textcert, Textgameover;
LButton MainscrPlay(510, 325, 150, 50), MainscrResume(510, 410, 150, 50), MainscrQuit(510, 495, 150, 50), MainscrHighscore(605, 595, 70, 70);
LButton ModeEasy(420, 240, 435, 85), ModeHard(420, 410, 435, 85), ModeBack(10, 10, 50, 50);

void loadMedia() {
    Textmainscr.loadFromFile( taolinkmenu("mainscr") ); //trangthai 0
    Textmode.loadFromFile( taolinkmenu("mode") ); //trangthai 1
    Textpause.loadFromFile( taolinkmenu("pause") );
    Texthighscore.loadFromFile( taolinkmenu("highscore") );
    Textcert.loadFromFile( taolinkmenu("cert") );
    Textgameover.loadFromFile( taolinkmenu("gameover") );
}

int main( int argc, char* args[] ){
    srand(time(NULL));
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
		return 0;
	}

	loadMedia();

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

	//xóa màn hình về màn hình trắng
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
    SDL_RenderClear( gRenderer );

    if(chedokho) maxHP=1;

    while( !quit ) {//vòng lặp chính của game
        SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
        SDL_RenderClear( gRenderer );

        while( SDL_PollEvent( &e ) != 0 ){
            if( e.type == SDL_QUIT ) {
                quit = true;
                break;
            }
            if(trangthai==0) {
//                Textmainscr.render();
                if(MainscrPlay.MouseClick(&e)) trangthai=1;
                if(MainscrResume.MouseClick(&e)) trangthai=1; //sửa sau
                if(MainscrQuit.MouseClick(&e)) quit=true;
                if(MainscrQuit.MouseClick(&e)) trangthai=1; //sửa sau
            }
            else if( trangthai == 1 ){
                if(ModeBack.MouseClick(&e)) trangthai=0;
                if(ModeEasy.MouseClick(&e)) trangthai=2, chedokho=false;
                if(ModeHard.MouseClick(&e)) trangthai=2, chedokho=true;
            }
        }

        if(trangthai==0) {
            SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
            SDL_RenderClear( gRenderer );
            Textmainscr.render();
            SDL_RenderPresent( gRenderer );
        }

        if(trangthai==1) {
            SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
            SDL_RenderClear( gRenderer );
            Textmode.render();
            SDL_RenderPresent( gRenderer );
        }

        if(trangthai==2) {
            frame=0;
            bool quit2=false;
            BuildMapStage1();

            while(!quit2) {
                Character *ndmaivy;
                for(int i=0; i<listnhanvat.size(); i++)
                    if(listnhanvat[i].id==1) ndmaivy = &listnhanvat[i];
                while( SDL_PollEvent( &e ) != 0 ){
                    if( e.type == SDL_QUIT ){
                        quit2 = true;
                        quit=true;
                        break;
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
                        if(e.key.keysym.sym == SDLK_SPACE) {
                            ndmaivy->attack=true;
                            continue;
                        }
                        if(e.key.keysym.sym == SDLK_f) {
                            UltiSun(ndmaivy);
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
                        if(e.key.keysym.sym == SDLK_SPACE) {
                            ndmaivy->attack=false;
                            continue;
                        }
                    }
                }

                SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
                SDL_RenderClear( gRenderer );

                if(frame==300) LuuDuLieu();
                if(frame==350) {
                    TaiDuLieu();
                    frame=350;
                }

                if(frame%400==0) {
                    int id=4;
                    CreateCharacter(id, 600, 496-h[id]);
                }

                for(int i=0; i<listnhanvat.size(); i++)
                    if(listnhanvat[i].id==1) HoatDongMVy(listnhanvat[i]);
                    else HoatDong(listnhanvat[i]);

                if(sunframe!=-1) {
                    LTexture Sun;
                    Sun.x = xsun;
                    Sun.y = ysun;
                    LoadSpriteCharacter(Sun, 7, "Sun", sunframe, delays, 0);

                    sunframe++;
                    if(sunframe==6*delays) sunframe=-1;
                }

                SDL_RenderPresent( gRenderer );
                frame++;
            }
        }
    }

	//Free resources and close SDL
	close();

	return 0;
}
