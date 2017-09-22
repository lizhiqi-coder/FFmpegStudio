precision mediump float;
varying vec2 vTextureCoord;
uniform sampler2D uTexture;


vec3 RGB2HSV(vec3 rgb){

        float R=rgb.x;
        float G=rgb.y;
        float B=rgb.z;

        float h;
        float s;
        float v;
        float max1=max(R,max(G,B));
        float min1=min(R,min(G,B));

        if(R==max1){
            h=(G-B)/(max1-min1);
        }
        if(G==max1){
            h=2.0+(B-R)/(max1-min1);
        }
        if(B==max1){
            h=4.0+(R-G)/(max1-min1);
        }
        h=h*60.0;
        if(h<0.0){
            h=h+360.0;
        }
        v=max1;
        s=(max1-min1)/max1;
        return vec3(h,s,v);

}

vec3 HSV2RGB(vec3 hsv){

        float R;
        float G;
        float B;
        if(hsv.y==0.0){
            R=G=B=hsv.z;
        }else{
            hsv.x=hsv.x/60.0;
//            int i=int(hsv.x);
            int i=int(hsv.x-6.0*floor(hsv.x/6.0));
            float f=hsv.x-float(i);
            float a=hsv.z*(1.0-hsv.y);
            float b=hsv.z*(1.0-hsv.y*f);
            float c=hsv.z*(1.0-hsv.y*(1.0-f));


            if(i==0){
                R=hsv.z;
                G=c;
                B=a;
            }else if(i==1){
                R=b;
                G=hsv.z;
                B=a;
            }else if(i==2){
                R=a;
                G=hsv.z;
                B=c;
            }else if(i==3){
                R=a;
                G=b;
                B=hsv.z;
            }else if(i==4){
                R=c;
                G=a;
                B=hsv.z;
            }else if(i==5){
                R=hsv.z;
                G=a;
                B=b;
            }
        }

        return vec3(R,G,B);
}


void main(){
    vec4 textureColor=texture2D(uTexture,vTextureCoord);
    float yuv_y=0.299*textureColor.r+0.587*textureColor.g+0.114*textureColor.b;
    float yuv_u=-0.169*textureColor.r-0.331*textureColor.g+0.499*textureColor.b+128.0;
    float out_color=yuv_y;

    vec3 textureHSV=RGB2HSV(vec3(textureColor.r,textureColor.g,textureColor.b));
    vec3 resultHSV=vec3(0,textureHSV.y,textureHSV.z);
    gl_FragColor=vec4(HSV2RGB(resultHSV),textureColor.a);


}