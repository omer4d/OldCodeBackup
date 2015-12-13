struct AllegAudio {
    int buffLen;
    AUDIOSTREAM* stream;
    Engine engine;
    List<int16_t*> history[2];
    int pos;
    
    AllegAudio(int logicRate, int buffLen, int freq): engine(512, freq)
    {
        this->buffLen = buffLen;
        this->stream = play_audio_stream(buffLen, 16, 1, freq, 255, 128);
        this->pos = 0;
    }
    
    ~AllegAudio()
    {
        stop_audio_stream(stream);
    }
    
    void render()
    {
        uint16_t* buff = (uint16_t*) get_audio_stream_buffer(stream);
        
        if(buff)
        {
            if(history[0].getSize() > 1)
            {
                int16_t* left = history[0].getFirst()->data;
                int16_t* right = history[1].getFirst()->data;
                
                for(int i = 0; i < buffLen; ++i, ++pos)
                {
                    if(pos == engine.getBuffLen())
                    {
                        pos = 0;
                        history[0].popFront();
                        history[1].popFront();
                        delete[] left;
                        delete[] right;
                        left = history[0].getFirst()->data;
                        right = history[1].getFirst()->data;
                    }
                    
                    buff[i * 2 + 0] = left[pos] - INT16_MIN;
                    buff[i * 2 + 1] = right[pos] - INT16_MIN;
                }
            }

            else
            {
                printf(".");
                for(int i = 0; i < buffLen; i++)
                {
                    buff[i * 2 + 0] = INT16_MAX;
                    buff[i * 2 + 1] = INT16_MAX;
                }
            }
            
            free_audio_stream_buffer(stream);
        }
    }
    
    void logic()
    {
        int n = engine.getBuffLen();
        int16_t* leftCopy = new int16_t[n];
        int16_t* rightCopy = new int16_t[n];
        
        memcpy(leftCopy, engine.getBuffer(0), n * 2);
        memcpy(rightCopy, engine.getBuffer(1), n * 2);
        
        engine.logic();
        history[0].pushBack(leftCopy);
        history[1].pushBack(rightCopy);
    }
};
