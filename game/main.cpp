#include<bits/stdc++.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
using namespace std;

const int MAIN_WIDTH = 3000;
const int MAIN_HEIGHT = 720;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

mt19937 rd(chrono::steady_clock::now().time_since_epoch().count());
#define rand rd;

int Rand(int l, int r) { // sinh 1 số ngẫu nhiên trong đoạn [l; r]
    return l+(1ll*rd()*rd()%(r-l+1) +(r-l+1))%(r-l+1);
}


int trangthai=0, frame=0, cntcharacter=0, score=0, maxHP=3, f[4*3005], sunframe=-1, xsun=-1, ysun=-1, luuHp=-1, luuammo=-1, enemydied=0;
bool chedokho=false, Musicc=true, endgame=false;
const int delays=6;

bool init();
void loadMedia();
void close();

//SDL_Window* gWindow = NULL;
//SDL_Renderer* gRenderer = NULL;
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

class Engine{
private:
    Engine() {}
    bool m_IsRunning;

    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    static Engine* s_Instance;
public:
    static Engine* GetInstance()
    {
        return s_Instance = (s_Instance != nullptr)? s_Instance : new Engine();
    }
    bool Init();
    void Quit();
    void Render();

    inline bool IsRunning()
    {
        return m_IsRunning;
    }
    inline SDL_Renderer* GetRenderer()
    {
        return m_Renderer;
    }
};

Engine* Engine::s_Instance = nullptr;

bool Engine::Init()
{
    SDL_Init( SDL_INIT_VIDEO );
    m_Window = SDL_CreateWindow( "MARIA AND THE MYSTERIOUS TREASURE", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    m_Renderer = SDL_CreateRenderer( m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    SDL_SetRenderDrawColor( m_Renderer,255,255,255,255);
}

void Engine::Quit()
{
    SDL_DestroyRenderer( m_Renderer );
    SDL_DestroyWindow( m_Window );
    m_Window = NULL;
    m_Renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

void Engine::Render(){
    SDL_RenderPresent(m_Renderer);
}

#define gRenderer  Engine::GetInstance()->GetRenderer()

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
		bool loadFromRenderedText( TTF_Font* gFont, string textureText, SDL_Color textColor );
		int getWidth();
		int getHeight();
		void SetRender();
        void SetTexture();

	private:
		SDL_Texture* mTexture;
		int mWidth;
		int mHeight;
};

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
//		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 255, 255, 255 ) );

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

void LTexture::SetTexture()
{
    mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, MAIN_WIDTH, MAIN_HEIGHT);
}

void LTexture::SetRender()
{
    SDL_SetRenderTarget(gRenderer,mTexture);
}

void LTexture::render( int x=0, int y=0, SDL_Rect* clip=NULL, bool flip=false){
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	if(!flip) SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
	else {
	    SDL_RendererFlip flipType= SDL_FLIP_HORIZONTAL;
        SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, 0.0, nullptr, flipType);
	}
}

bool LTexture::loadFromRenderedText( TTF_Font* gFont, string textureText, SDL_Color textColor ){
    //Get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }

    return mTexture != NULL;
}

class LButton
{
	public:
	    int x, y, w, h;
		LButton();
		LButton(int xx, int yy, int ww, int hh);
		bool MouseClick( SDL_Event *e );
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

class Dot {
    public:
        static const int DOT_WIDTH = 640;
        static const int DOT_HEIGHT = 20;
        static const int DOT_VEL = 20;
        Dot();
        void handleEvent( SDL_Event& e );
        void move();
        int getPosX();
        int setPosX(int x);
    private:
        int mPosX;
        int mDeltaX;
};

Dot::Dot(){
    mPosX = 0;
    mDeltaX = 0;
}

void Dot::handleEvent( SDL_Event& e ){
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        if(e.key.keysym.sym      == SDLK_LEFT)  mDeltaX -= DOT_VEL;
        else if(e.key.keysym.sym == SDLK_RIGHT) mDeltaX += DOT_VEL;
    }
    else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        if(e.key.keysym.sym      == SDLK_LEFT)  mDeltaX += DOT_VEL;
        else if(e.key.keysym.sym == SDLK_RIGHT) mDeltaX -= DOT_VEL;
    }
}

int Dot::getPosX(){
    return mPosX;
}

int Dot::setPosX(int x){
    mPosX=x;
}

void Dot::move(){
    mPosX += mDeltaX;
    if( mPosX<0 || (mPosX+DOT_WIDTH)>3000 ) mPosX -= mDeltaX;
}

Dot dot;

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
int w[] =           {-1,  48,  64,  48,  64,  128,  12,  11,  80,  50,       50};
int h[] =           {-1,  96,  64,  48,  64,  64,   7,   11,  50,  50,       50};
int HPs[] =         {-1,  3,   1,   10,  2,   5,    2,   2,   2,   1272005,  5002721};
int atks[] =        {-1,  1,   1,   1,   1,   1,    0,   0,   0,   0,        0};
int speeds[] =      {-1,  3,   2,   1,   0,   2,    0,   0,   0,   0,        0};
int CanAttacks[] =  {-1,  1,   0,   0,   1,   1,    5,   5,   0,   0,        0};
///dữ liệu objects (không cần mảng atk dành riêng cho đạn)
int w2[] =          {-1,  30,  20,  16,  10,  16,  16,  16,  32,  20,  17,  25,  30};
int h2[] =          {-1,  30,  25,  17,  20,  16,  16,  16,  20,  20,  17,  15,  30};
int speed2[] =      {-1,  0,   0,   0,   0,   0,   0,   0,   0,   7,   7,   7,   0};

LTexture Texturebackground2;
LTexture Textmainscr, Textmode, Textpause, Texthighscore, Textcert, Textgameover, soil, grass, mainback, TextScore;
LButton MainscrPlay(510, 325, 150, 50), MainscrResume(510, 410, 150, 50), MainscrQuit(510, 495, 150, 50), MainscrHighscore(605, 595, 70, 70);
LButton ModeEasy(420, 240, 435, 85), ModeHard(420, 410, 435, 85), ModeBack(10, 10, 50, 50);
LButton HighscoreBack(10, 10, 50, 50);
LButton PauseHome(500, 330, 60, 75), PauseHighscore(605, 335, 65, 75), PauseQuit(710, 340, 60, 65), PauseResume(510, 440, 155, 50);
LButton GameoverHome(510, 410, 60, 65), GameoverHighscore(610, 410, 60, 65), GameoverQuit(710, 410, 40, 70);
LButton CertHome(530, 630, 130, 50);
LButton Speaker(1210, 10, 60, 60);
TTF_Font* FontScore = NULL;
TTF_Font* FontRecord = NULL;
Mix_Music *MainMusic = NULL;
Mix_Chunk *SoundClick= NULL, *SoundJump= NULL, *SoundShot=NULL, *SoundUlti=NULL, *SoundCoin=NULL;

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
    if(chedokho) maxHP=1;
    else maxHP=3;
    if(id==1) {aaa.HP = min(aaa.HP, maxHP); dot.setPosX( aaa.x );}
    if(id!=1) updatesegment(1, 1, 3000, (aaa.x+aaa.u)/2, 1);
    if(id==1 && luuHp!=-1 && luuammo!=-1) {
        aaa.HP=luuHp;
        aaa.ammo=luuammo;
    }
    listnhanvat.push_back(aaa);
}

void ClearCharacter(int thutu) { //xóa character
    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu == thutu) {
            if(listnhanvat[i].id==2) score+=50;
            if(listnhanvat[i].id==3) score+=70;
            if(listnhanvat[i].id==4) score+=100;
            if(listnhanvat[i].id==5) score+=120;
            listnhanvat.erase(listnhanvat.begin() + i);
        }
    enemydied++;
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

string taolinkmenu(string namee, string duoi=".png") { //tạo link ảnh vật phẩm
    string res="menu/";
    res+=namee;
    res+=duoi;
    return res;
}

string taolinksound(string namee) {
    string res="sound/";
    res+=namee;
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
    if(id==8) numsheets=4;

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

        if(listvatpham[i].x <-100 || listvatpham[i].x >3100) {
            ClearObject(listvatpham[i].thutu);
            continue;
        }

        LoadSpriteObject(listvatpham[i].Text, listvatpham[i].id, frame, delays, listvatpham[i].huong);

        if(GiaoNhau(listvatpham[i].x, listvatpham[i].u, ndmaivy.x, ndmaivy.u))
            if(GiaoNhau(listvatpham[i].y, listvatpham[i].v, ndmaivy.y, ndmaivy.v)) {

                if(listvatpham[i].id == 2) ndmaivy.ammo = min(5, ndmaivy.ammo+2), score+=100;
                if(listvatpham[i].id == 3) ndmaivy.HP = min(maxHP, ndmaivy.HP+1), score+=200;
                if(listvatpham[i].id == 4) {ndmaivy.buff=true; ndmaivy.buffframe=0; score+=50;}
                if(listvatpham[i].id == 5) score+=10;
                if(listvatpham[i].id == 6) score+=100;
                if(listvatpham[i].id == 7) score+=1000;
                if(listvatpham[i].id <= 7) Mix_PlayChannel( -1, SoundCoin, 0);
                if(9 <= listvatpham[i].id && listvatpham[i].id<=11) ndmaivy.HP--;
                if(listvatpham[i].id == 12) {
                    endgame=true;
                    Mix_PlayChannel( -1, SoundCoin, 0);
                }

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
        newy= doituong.v - 5 - h2[id];
        CreateObject(id, newx, newy);
    }
}

void HoatDongMVy(Character &ndmaivy) {//chạy nhân vật chính
    string thaotac="Idle";

    ndmaivy.idleframe++;
    int deltax=ndmaivy.dx, deltay=6;
    int doituongbigiam=-1;

///~~~~~~xử lý di chuyển theo phương x~~~~~~~~~~~~

    for(int i=0; i<listnhanvat.size(); i++)
        if(listnhanvat[i].thutu != ndmaivy.thutu) {

            if(listnhanvat[i].id==8)
                if(KhoangCach(ndmaivy.x, ndmaivy.u, listnhanvat[i].x, listnhanvat[i].u) <= 10) listnhanvat[i].HP=-1111;


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
//    if(ndmaivy.v==496) {OnObject=true; deltay=0;}

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

        if(ndmaivy.jumpframe==0) Mix_PlayChannel( -1, SoundJump, 0 );

        if(ndmaivy.jumpframe < 12) deltay = -6;
        else if(ndmaivy.jumpframe < 24) deltay = -3;
        else if(ndmaivy.jumpframe < 36) deltay = 3;
        else deltay=6;

        ndmaivy.jumpframe++;
        if(ndmaivy.jumpframe == 48) {
            ndmaivy.jumpframe = 0;
        }
    }
    else if(!OnObject) { //falling
        deltay=min(deltay, 6);
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
    if(deltax!=0) dot.setPosX( ndmaivy.x );

    ndmaivy.y += deltay;
    ndmaivy.v += deltay;
    ndmaivy.Text.y += deltay;

    if(ndmaivy.y >= 720) ndmaivy.HP=0;// rơi lố -> chết

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
        Mix_PlayChannel( -1, SoundShot, 0 );
        ndmaivy.ammo--;
    }
    if(ndmaivy.reloadframe!=0) ndmaivy.reloadframe++;
    if(ndmaivy.reloadframe==50) ndmaivy.reloadframe=0;
    if(frame%500==0) ndmaivy.ammo=min(ndmaivy.ammo+1, 5); //tự động tăng đạn

    if(ndmaivy.idleframe>=336 && ndmaivy.id==1) thaotac="Spin";

    HoatDongVatPham(ndmaivy); ///chạy vật phẩm

///~~~~~~~rendering~~~~~~~~~~~~~~~~~

    if(thaotac=="Jump") LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, ndmaivy.jumpframe, delays, ndmaivy.huong);
    else LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, thaotac, frame, delays, ndmaivy.huong);

    if(ndmaivy.buff) LoadSpriteCharacter(ndmaivy.Text, ndmaivy.id, "Heart", frame, delays, ndmaivy.huong);
}

void HoatDong(Character &doituong) {//chạy character phụ
    HoatDongVatPham2(doituong);

///=============vật cản, rương==================================================================================

    if(doituong.id>=6) {
        if(doituong.id == 6) LoadSpriteCharacter(doituong.Text, 6, "pipe", frame, delays, 0, doituong.u-doituong.x, doituong.v-doituong.y);
        if(doituong.id == 8) {
            LoadSpriteCharacter(doituong.Text, 8, "chest", doituong.deathframe, 12, 0);
            if(doituong.HP<=-1111) {
                doituong.deathframe++;
                if(doituong.deathframe==48) {
                    if(trangthai<=7) {
                        CreateObject(2, doituong.x+5, doituong.v-10-h2[2]);
                        CreateObject(3, doituong.u-5-w2[3], doituong.v-10-h2[3]);
                    }
                    else CreateObject(12, (doituong.x+doituong.u)/2 - 15, (doituong.y+doituong.v)/2 - 15 );
                    ClearCharacter(doituong.thutu);
                }
            }
        }
        if(doituong.id==9) grass.render(doituong.x, doituong.y);
        if(doituong.id==10) soil.render(doituong.x, doituong.y);
        return;
    }

///=============phần của quái===================================================================================
    if(doituong.HP<=0) {
//        if(doituong.deathframe==0) Mix_PlayChannel(-1, SoundCoin, 0);

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
            if(listnhanvat[i].x > ndmaivy->x+1000 || listnhanvat[i].x < ndmaivy->x-500) continue;
            int mid=(listnhanvat[i].x+listnhanvat[i].u)/2;
            int valuee=getsegment(1, 1, 3000, max(1, mid - rangee), mid);
            if(valuee>val) {
                val=valuee;
                bestt = i;
            }
        }

    if(bestt != -1) { //đảm bảo ulti giết được quái
        int mid=(listnhanvat[bestt].x+listnhanvat[bestt].u)/2;
        ndmaivy->ammo -= 5;
        sunframe=0;
        Mix_PlayChannel( -1, SoundUlti, 0 );

        int r=mid, l=mid;

        for(int i=0; i<listnhanvat.size(); i++)
        if(2<=listnhanvat[i].id && listnhanvat[i].id<=5) {
            int mid2=(listnhanvat[i].x+listnhanvat[i].u)/2;
            if(mid-rangee<= mid2 && mid2<= mid) {
                l=min(l, mid2);
                listnhanvat[i].HP=0;
            }
        }

        xsun=(l+r)/2 -50; //để tâm của mặt trời nằm giữa khoảng của những kẻ địch bị chọn bị trừng phạt
        ysun=listnhanvat[bestt].y-400; //để mặt trời trên cao
    }
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

void ClearData() {
    frame=0;
    score=0;
    listnhanvat.clear();
    listvatpham.clear();
}

void WhiteData() {
    ofstream outt("data.txt");

    outt<<1<<" "<<0<<" "<<0<<" "<<0<<" "<<5<<" "<<-1<<" "<<-1<<" "<<-1<<" "<<0<<" "<<1<<"\n";
    outt<<0<<"\n";
    outt<<0<<"\n";

    listnhanvat.clear();
    listvatpham.clear();

    outt.close();
}

void PrintRecord() {
    Texthighscore.render();
    SDL_Color PinkColor= {255,107,170};
    ifstream inp("highscore.txt");
    string number;

    int pos=200;
    for(int i=0; i<5; i++){
        getline(inp, number);
//        while(number.length()<9) number=" "+number;
        TextScore.loadFromRenderedText( FontRecord, number, PinkColor);
        TextScore.render(635- TextScore.getWidth()/2 , pos+i*80);
    }
    inp.close();
}

void UpdateHighscore() {
    ifstream inp("highscore.txt");
    int scoree[8];
    for(int i=1; i<=5; i++) inp>>scoree[i];
    inp.close();

    ofstream outt("highscore.txt");
    if(!chedokho) scoree[6]=score;
    else scoree[6]=3*score;
    sort(scoree+1, scoree+6+1);
    for(int i=6; i>=2; i--) {
        outt<<scoree[i]<<"\n";
    }
    outt.close();
}

bool LButton::MouseClick( SDL_Event* e ){
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
			if( e->type == SDL_MOUSEBUTTONDOWN) {
				Mix_PlayChannel( -1, SoundClick, 0 );
				return true;
			}
		}
	}
	return false;
}

void TatBatNhac() {
    Musicc = 1-Musicc;

    if( Mix_PausedMusic() == 1 ) Mix_ResumeMusic();
	else Mix_PauseMusic();
}

void loadMedia() {
    Textmainscr.loadFromFile( taolinkmenu("mainscr") ); //trangthai 0
    Textmode.loadFromFile( taolinkmenu("mode") ); //trangthai 1
    Texthighscore.loadFromFile( taolinkmenu("highscore") ); //trangthai 2
    Textpause.loadFromFile( taolinkmenu("pause") ); //trang thai 3
    Textgameover.loadFromFile( taolinkmenu("gameover") ); //trangthai 4
    Textcert.loadFromFile( taolinkmenu("cert") ); //trang thai 5
    //trang thai 6+ là cac man
    mainback.loadFromFile( taolinkmenu("mainback") );
    soil.loadFromFile( taolinkmenu("soil") );
    grass.loadFromFile( taolinkmenu("grass") );

    Texturebackground2.SetTexture();

    FontScore = TTF_OpenFont( taolinkmenu("Baloo-Regular", ".ttf").c_str(), 36 );
    FontRecord= TTF_OpenFont( taolinkmenu("Baloo-Regular", ".ttf").c_str(), 70 );

    MainMusic = Mix_LoadMUS( taolinksound("vntrack00.wav").c_str() ); //vntrack00
    SoundClick= Mix_LoadWAV( taolinksound("click.wav").c_str() );
    SoundJump = Mix_LoadWAV( taolinksound("jumpbig.wav").c_str() );
    SoundShot = Mix_LoadWAV( taolinksound("shot.wav").c_str() );
    SoundUlti = Mix_LoadWAV( taolinksound("ulti.wav").c_str() );
    SoundCoin = Mix_LoadWAV( taolinksound("coin.wav").c_str() );
}

void VeRoiRac(Character *ndmaivy){
    //show HP of main character
    LTexture heartt;
    for(int i=1; i<=ndmaivy->HP; i++) {
        heartt.x=(i-1)*40+10;
        heartt.y=10;
        LoadSpriteObject(heartt, 1, frame, delays);
    }

    //show ammo of main character
    LTexture ammoo;
    for(int i=1; i<=ndmaivy->ammo; i++) {
        ammoo.x=(i-1)*20+10;
        ammoo.y=50;
        LoadSpriteObject(ammoo, 13, frame, delays);
    }

    //showscore
    string number= "Score: "+NumToString(score);
    SDL_Color LightGoldColor= {255, 253, 132};
    TextScore.loadFromRenderedText( FontScore, number, LightGoldColor);
    TextScore.render(1280-TextScore.getWidth(), 0);
}

void Run_dot(Character *ndmaivy){
    dot.move();
    camera.x = ( dot.getPosX() + Dot::DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
    camera.y = 0;
    if( camera.x < 0 ) camera.x = 0;
    if( camera.y < 0 ) camera.y = 0;
    if( camera.x > MAIN_WIDTH - camera.w ) camera.x = MAIN_WIDTH - camera.w;
    if( camera.y > MAIN_HEIGHT - camera.h ) camera.y = MAIN_HEIGHT - camera.h;
}

void BuildMapStage1() {
//    listnhanvat.clear();
//    listvatpham.clear();
    frame=0;
    cntcharacter=0;
    score=0;

    ///Địa hình
    for(int i=0; i<62; i++) {
        if(12<=i && i<=13) {
            CreateCharacter(9, i*50, 570);
            CreateCharacter(9, i*50, 620);
            CreateCharacter(10, i*50, 670);
            continue;
        }
        if(i==28 || i==31) {
            CreateCharacter(6, i*50, 550, 50, 70);
            CreateCharacter(9, i*50, 620);
            CreateCharacter(10, i*50, 670);
            continue;
        }
        if(29<=i && i<=30) continue;
        if(41<=i && i<=43) {
            if(i==42) CreateCharacter(9, i*50, 520);
            CreateCharacter(9, i*50, 570);
            CreateCharacter(9, i*50, 620);
            CreateCharacter(10, i*50, 670);
            continue;
        }
        if(52<=i && i<=55) {
            for(int k=1; k<=4; k++)
                for(int p=1; p<=k; p++) CreateCharacter(9, 2600+(k-1)*50, 670-p*50);

            CreateCharacter(10, i*50, 670);
            continue;
        }

        CreateCharacter(9, i*50, 620);
        CreateCharacter(10, i*50, 670);
    }

    ///Characters
    CreateCharacter(1, 10, 620-h[1]);
    CreateCharacter(2, 270, 620-h[2]);
    CreateCharacter(2, 530, 620-h[2]);
    CreateCharacter(2, 800, 620-h[2]);
    CreateCharacter(2, 1000, 620-h[2]);
    CreateCharacter(3, 1240, 620-h[3]);
    CreateCharacter(3, 1700, 620-h[3]);
    CreateCharacter(2, 1850, 620-h[2]);
    CreateCharacter(3, 2280, 620-h[3]);
    CreateCharacter(3, 2550, 620-h[3]);
    CreateCharacter(2, 2820, 620-h[2]);
    CreateCharacter(8, 2925, 620-h[8]);

    ///Objects
    CreateObject(4, 650, 565-h2[4]);
    CreateObject(2, 2115, 515-h2[2]);
    CreateObject(3, 2710, 515-h2[3]);
}

void BuildMapStage2() {
    listnhanvat.clear();
    listvatpham.clear();
    frame=0;
    cntcharacter=0;

    ///Địa hình
    for(int i=0; i<62; i++) {
        if(13<=i && i<=16){
            if(i==14 || i==15) continue;
            CreateCharacter(9, i*50, 570);
        }

        if(30<=i && i<=33){
            if(i==31 || i==32) continue;
            CreateCharacter(6, i*50, 550, 50, 70);
        }

        if(i==41 || i==42) CreateCharacter(9, i*50, 570);
        if(i==42) CreateCharacter(9, i*50, 520);

        if(i==52) CreateCharacter(6, i*50, 550, 50, 70);

        CreateCharacter(9, i*50, 620);
        CreateCharacter(10, i*50, 670);
    }

    ///Characters
    CreateCharacter(1, 10, 620-h[1]);
    CreateCharacter(3, 300, 620-h[3]);
    CreateCharacter(3, 470, 620-h[3]);
    CreateCharacter(2, 620, 620-h[2]);
    CreateCharacter(2, 900, 620-h[2]);
    CreateCharacter(3, 1260, 620-h[3]);
    CreateCharacter(4, 1435, 620-h[4]);
    CreateCharacter(4, 1490, 550-h[4]);
    CreateCharacter(3, 1750, 620-h[3]);
    CreateCharacter(2, 1850, 620-h[2]);
    CreateCharacter(4, 2035, 570-h[4]);
    CreateCharacter(2, 2260, 620-h[2]);
    CreateCharacter(4, 2590, 550-h[4]);
    CreateCharacter(3, 2700, 620-h[3]);
    CreateCharacter(3, 2820, 620-h[3]);

    CreateCharacter(8, 2925, 620-h[8]);

    ///Objects
    CreateObject(4, 670, 565-h2[4]);
    CreateObject(2, 1000, 615-h2[2]);
    CreateObject(3, 2115, 515-h2[3]);
}

void BuildMapStage3() {
    listnhanvat.clear();
    listvatpham.clear();
    frame=0;
    cntcharacter=0;

    ///Địa hình
    for(int i=0; i<62; i++) {
        if(6<=i && i<=11) CreateCharacter(9, i*50, 570);
        if(7<=i && i<=11) CreateCharacter(9, i*50, 520);

        if(12<=i && i<=13) continue;

        if(14<=i && i<=21) CreateCharacter(9, i*50, 570);
        if(i==14) CreateCharacter(6, i*50, 500, 50, 70);
        if(i==20) CreateCharacter(9, i*50, 520);

        if(i==37) CreateCharacter(6, i*50, 550, 50, 70);

        if(i==44) CreateCharacter(9, i*50, 570);

        if(i==47 || i==48) continue;

        if(i==51)CreateCharacter(9, i*50, 570);
        if(i==52)
            for(int p=1; p<=3; p++)
                for(int q=1; q<=p; q++) CreateCharacter(9, (i+p-1)*50, 620-q*50);


        CreateCharacter(9, i*50, 620);
        CreateCharacter(10, i*50, 670);
    }

    ///Characters
    CreateCharacter(1, 10, 620-h[1]);
    CreateCharacter(3, 230, 620-h[3]);
    CreateCharacter(4, 530, 520-h[4]);
    CreateCharacter(5, 900, 570-h[5]);
    CreateCharacter(5, 1110, 620-h[5]);
    CreateCharacter(3, 1280, 620-h[3]);
    CreateCharacter(4, 1780, 620-h[4]);
    CreateCharacter(3, 1960, 620-h[3]);
    CreateCharacter(5, 2090, 620-h[5]);
    CreateCharacter(4, 2485, 620-h[4]);
    CreateCharacter(4, 2560, 570-h[4]);
    CreateCharacter(4, 2635, 520-h[4]);
    CreateCharacter(2, 2800, 620-h[2]);

    CreateCharacter(8, 2925, 620-h[8]);

    ///Objects
    CreateObject(2, 1015, 515-h2[2]);
    CreateObject(3, 1600, 615-h2[3]);
    CreateObject(7, 2715, 465-h2[7]);
}

int main( int argc, char* args[] ){
    srand(time(NULL));

    //Mix_OpenAudio( 45100, MIX_DEFAULT_FORMAT, 2, 4096 ); //try 44100, 48000, 54100, 22050 if your laptop is not ndmaivy's one =)))))
    Engine::GetInstance()->Init();
    TTF_Init();
    loadMedia();
	if( Mix_PlayingMusic() == 0 ) Mix_PlayMusic( MainMusic, 0 );

    bool quit = false;
    SDL_Event e;
    int pretrangthai=0; //phục vụ cho nút back trong highscore

    while( !quit ) {//vòng lặp chính của game
        SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
        SDL_RenderClear( gRenderer );
        bool continuee=false;

        if( Mix_PlayingMusic() == 0 ) Mix_PlayMusic( MainMusic, -1 );

        while( SDL_PollEvent( &e ) != 0 ){ //xử lý mouse_event ở các màn hình thao tác bằng chuột
            if( e.type == SDL_QUIT ) {
                quit = true;
                break;
            }
            if(trangthai==0) {
                if(MainscrPlay.MouseClick(&e)) trangthai=1;
                if(MainscrResume.MouseClick(&e)) {
                    TaiDuLieu();
                    continuee=true;
                }
                if(MainscrQuit.MouseClick(&e)) quit=true;
                if(MainscrHighscore.MouseClick(&e)) {
                    pretrangthai=0;
                    trangthai=2;
                }
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
            else if(trangthai==1){
                if(ModeBack.MouseClick(&e)) trangthai=0;
                if(ModeEasy.MouseClick(&e)) trangthai=6, chedokho=false;
                if(ModeHard.MouseClick(&e)) trangthai=6, chedokho=true;
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
            else if(trangthai==2) {
                if(HighscoreBack.MouseClick(&e)) trangthai=pretrangthai;
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
            else if(trangthai==3) {
                if(PauseHome.MouseClick(&e)) trangthai=0;
                if(PauseHighscore.MouseClick(&e)) {
                    pretrangthai=3;
                    trangthai=2;
                }
                if(PauseQuit.MouseClick(&e)) quit=true;
                if(PauseResume.MouseClick(&e)) {
                    TaiDuLieu();
                    continuee=true;
                }
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
            else if(trangthai==4) {
                if(GameoverHome.MouseClick(&e)) trangthai=0;
                if(GameoverHighscore.MouseClick(&e)) {
                    pretrangthai=4;
                    trangthai=2;
                }
                if(GameoverQuit.MouseClick(&e)) quit=true;
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
            else if(trangthai==5) {
                if(CertHome.MouseClick(&e)) trangthai=0;
                if(Speaker.MouseClick(&e)) TatBatNhac();
            }
        }

        if(trangthai<=5) {
            SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
            SDL_RenderClear( gRenderer );

            if(trangthai==0) Textmainscr.render();
            if(trangthai==1) Textmode.render();
            if(trangthai==2) PrintRecord();
            if(trangthai==3) Textpause.render();
            if(trangthai==4) Textgameover.render();
            if(trangthai==5) Textcert.render();

            LTexture speakerr;
            if(Musicc) speakerr.loadFromFile( taolinkmenu("Icon_SoundOn") );
            else speakerr.loadFromFile( taolinkmenu("Icon_SoundOff") );
            speakerr.render(1210, 10);

            Engine::GetInstance()->Render();
        }

        if(trangthai>=6) {
            bool quit2=false;

            if(!continuee && trangthai==6) BuildMapStage1(); //LuuDuLieu();}
            if(!continuee && trangthai==7) BuildMapStage2(); //LuuDuLieu();}
            if(!continuee && trangthai==8) BuildMapStage3(); //LuuDuLieu();}
            continuee=false;

            while(!quit2) {
                if( Mix_PlayingMusic() == 0 ) Mix_PlayMusic( MainMusic, 0 );

                Character *ndmaivy;
                for(int i=0; i<listnhanvat.size(); i++)
                    if(listnhanvat[i].id==1) ndmaivy = &listnhanvat[i];

                while( SDL_PollEvent( &e ) != 0 ){ //xử lý key_event điều khiển gameplay
                    dot.handleEvent(e);
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
                        if(e.key.keysym.sym == SDLK_p) {
                            ndmaivy->dx=0;
                            ndmaivy->jump=false;
                            ndmaivy->attack=false;
                            LuuDuLieu();
                            trangthai=3;
                            quit2=true;
                            break;
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

                SDL_RenderClear(gRenderer);
                Run_dot(ndmaivy);
                Texturebackground2.SetRender();
                SDL_RenderClear(gRenderer);
                mainback.render(0,0);

                for(int i=0; i<listnhanvat.size(); i++) {
                    if(listnhanvat[i].id==1) HoatDongMVy(listnhanvat[i]);
                    else HoatDong(listnhanvat[i]);

                    i -= enemydied; //do vector khi erase 1 phần tử sẽ đẩy các phần tử khác lên thế chỗ
                    enemydied = 0;
                }

                if(sunframe!=-1) { //chạy hoạt ảnh cho ulti
                    LTexture Sun;
                    Sun.x = xsun;
                    Sun.y = ysun;
                    LoadSpriteCharacter(Sun, 7, "Sun", sunframe, delays, 0);

                    sunframe++;
                    if(sunframe==6*delays) sunframe=-1;
                }

                if(ndmaivy->HP <=0) { //nhân vật chính hết máu -> thua
                    trangthai=4;
                    ClearData();
                    quit2=true;
                }

                if(ndmaivy->x > 3000) { //qua màn
                    trangthai++;
                    luuHp=ndmaivy->HP;
                    luuammo=ndmaivy->ammo;
                    quit2=true;
                }

                if(endgame) { //nhặt cert
                    trangthai=5;
                    quit2=true;
                    UpdateHighscore();
                    WhiteData();
                    endgame=false;
                }

                SDL_SetRenderTarget(gRenderer,nullptr);
                SDL_RenderClear(gRenderer);
                Texturebackground2.render(0,0,&camera);
                VeRoiRac(ndmaivy); //Vẽ máu, đạn, score

                Engine::GetInstance()->Render();
                frame++;
            }
        }
    }

	Engine::GetInstance()->Quit();

	Mix_FreeMusic( MainMusic );
	MainMusic = NULL;

	Mix_Quit();

	return 0;
}
