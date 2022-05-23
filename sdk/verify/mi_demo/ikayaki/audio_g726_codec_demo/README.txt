audio_726_codec_demo使用说明:

运行：
    ./prog_audio_g726_codec_demo  (Enter files and paths)  (Output files and paths) (operation)
    
    operation选择如下:
        1.encode_to16k
        2.encode_to24k
        3.encode_to32k 
        4.encode_to40k
        5.decode

    例如：
        将8K_16bit_MONO_30s.wav的原始数据编码为比特率为16k的数据
            ./prog_audio_g726_codec_demo ./8K_16bit_MONO_30s.wav ./G726_16_8K_16bit_MONO_30s.wav encode_to16k
        将G726_16_8K_16bit_MONO_30s.wav编码后的数据解码输出    
            ./prog_audio_g726_codec_demo ./G726_16_8K_16bit_MONO_30s.wav ./G726_16_8K_16bit_MONO_30s_decode.wav decode

注：在sdk/verify/mi_demo/source/audio_g726_codec_demo有音频文件可用于测试
