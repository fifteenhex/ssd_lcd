audio_711_codec_demo使用说明:
运行：
    ./prog_audio_g711_codec_demo  (Enter files and paths)  (Output files and paths) (operation)
    
    operation选择如下:
        1.en_linear2alaw
        2.en_linear2ulaw
        3.de_alaw2linear 
        4.de_ulaw2linear
        5.con_alaw2ulaw
        6.con_ulaw2alaw
    例如：
        ./prog_audio_g711_codec_demo ./8K_16bit_MONO_30s.wav ./G711A_8K_16bit_MONO_30s.wav en_linear2alaw

注：在sdk/verify/mi_demo/source/audio_g711_codec_demo有音频文件可用于测试
