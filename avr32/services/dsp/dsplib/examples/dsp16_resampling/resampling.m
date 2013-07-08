function [w] = resampling(u,Fsout,Fsin)
    % ---------- L and M calculation ---------
    M = Fsin/(dsp_op_gcd(Fsin,Fsout));
    L = Fsout/(dsp_op_gcd(Fsin,Fsout));    
    % ----------------------------------------


    % ---------- Interpolation --------------
    % -- Step 1/ Up-Sampling
    vp = dsp_up_sample(u,L);
    
    % -- Step 2/ Low Pass FIR filtering
    % -- AVR32 Dsp Implementation   
    FIR_COEF_SIZE = 6 * L; 
    h = fir1(FIR_COEF_SIZE-1,1/M,kaiser(FIR_COEF_SIZE,7.8562));
    v = conv(h, vp);   
    v = v(FIR_COEF_SIZE:length(v));
    
    % -- Step 3/ Down-Sampling
    w = dsp_down_sample(v,M);