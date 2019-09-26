#include <pixels.h>

PIXELS::PIXELS(){} // I'll do something with this, I swear.

bool PIXELS::receive(uint8_t *pyld, unsigned length){
    uint16_t pixCnt = 0;
    uint8_t chan = 0;
    pixel *pattern = unmarshal(pyld, length, &pixCnt, &chan);
        
    if(pixCnt==0){
        show(chan);
        Serial.println("Clearing strand");
        return true;
    }
    /*
    for(uint i=0; i<pixCnt; i++){
        Serial.print("Got LED value RGB(");
        Serial.print(pattern[i].R);
        Serial.print(",");
        Serial.print(pattern[i].G);
        Serial.print(",");
        Serial.print(pattern[i].B);
        Serial.println(")");
    }
    */
    show(pattern, pixCnt, chan);
    delete pattern;
    return true;
}

void PIXELS::write(unsigned location, uint8_t R, uint8_t G, uint8_t B, uint8_t W){
    /*
    #ifdef RGBW
    strip.SetPixelColor(location, RgbwColor(R,G,B,W));
    #else
    strip.SetPixelColor(location, RgbColor(R,G,B));
    #endif
    */
}

void PIXELS::show(uint8_t chan){
    for(unsigned i=0; i<channel_cnt; i++){
        if(channels[i].chan==chan){
            channels[i].bus->Show();
            return;
        }
    }
}

void PIXELS::show(pixel *pixels, unsigned cnt, uint8_t chan){
    for(unsigned i=0; i<channel_cnt; i++){
        if(channels[i].chan==chan){
            for(unsigned i = 0; i<cnt; i++){
                #if RGBW
                channels[i].bus->SetPixelColor(i, RgbwColor(pixels[i].R,pixels[i].G,pixels[i].B,pixels[i].W));
                #else
                channels[i].bus->SetPixelColor(i, RgbColor(pixels[i].R,pixels[i].G,pixels[i].B));
                #endif
            }
            channels[i].bus->Show();
            return;
        }
    }
}

bool PIXELS::addChannel(int dataPin, unsigned cnt, uint8_t chan){
    NeoPixelBus<COLORMODE, DRIVERMETHOD> bus(cnt, dataPin);
    channel c = {&bus, chan};
    channel *n = new channel[channel_cnt+1];
    if(!n){
        channel_cnt;
        return false;
    }
    c.bus->Begin();
    memcpy(n, channels, sizeof(channel[channel_cnt]));
    channels = n;
    channels[channel_cnt++] = c; // Ensure this line works properly, should place pointer to channel at end of array than inc number of channels to the correct cnt

    return true;
}

pixel *PIXELS::unmarshal(uint8_t *pyld, unsigned len, uint16_t *pixCnt, uint8_t *channel){
    if(pyld[0]!=0x50){
        Serial.println("Missing checkvalue");
        // Set pixCnt to zero as we have not decoded any pixels and return NULL
        *pixCnt = 0;
        return NULL;
    }
    //if (pyld[1]!=syncWord||syncWord==0x0){
    //    *pixCnt = 0;
    //    return NULL;
    //} TODO UNCOMMENT REMOVED FOR EASY NETCAT USAGE QUICKLY
    if(channel!=NULL){
        *channel = pyld[2];
    }
    // Decode number of pixels, we don't have to send the entire strip if we don't want to
    uint16_t cnt = pyld[3] | pyld[4]<<8;

    pixel *result = new pixel[cnt];
    // TODO Add logic to return if len is impossibly large or small
    for(uint16_t i = 0; i<cnt; i++){
        #ifdef RGBW
        result[i].R = pyld[5+(i*4)];
        result[i].G = pyld[5+(i*4)+1];
        result[i].B = pyld[5+(i*4)+2];
        result[i].W = pyld[5+(i*4)+3];
        #else
        result[i].R = pyld[5+(i*3)];
        result[i].G = pyld[5+(i*3)+1];
        result[i].B = pyld[5+(i*3)+2];
        #endif
    }


    // TODO Add CRC check before setting pixCnt
    *pixCnt = cnt;
    return result; 
}

