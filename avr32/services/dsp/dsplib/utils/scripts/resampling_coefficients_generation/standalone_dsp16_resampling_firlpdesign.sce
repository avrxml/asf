mode(0);

// Configuration
fs_in_hz = 44100;
fs_out_hz = 48000;
order = 30;
normalized = 1;
output_path = "./";

function coefs = custom_filter(fc_hz, fs_hz, order)
  coefs = wfir('lp', order, [fc_hz / fs_hz, 0], 'hn', [0 0]);
endfunction

// Use LF as End Of Line character (Unix format)
EOL = char(10);

// Generate L and M coefficients
factor_m = fs_in_hz / double(gcd(int32([fs_in_hz, fs_out_hz])));
factor_l = fs_out_hz /  double(gcd(int32([fs_in_hz, fs_out_hz])));

// Set the cut-off frequency for the filter
fc_hz = min(fs_in_hz, fs_out_hz) / 2;

// Generate the FIR coefficients
coef = custom_filter(fc_hz, fs_in_hz * factor_l, order * factor_l);

// Normalize the coefficients if needed
// To normalize it uses the fact that the resampling algorithm uses this filter as a FIR polyphase filter.
if normalized == 1 then
  max_val = 0;
  for i = 0:(factor_l - 1),
    sum_val = 0;
    for j = 1:factor_l:(order * factor_l),
      sum_val = sum_val + abs(coef(j + i));
    end
    max_val = max(max_val, sum_val);
  end
  coef = coef / max_val;
end

// Sort the coefficients
max_val = 1;
k = 1;
sorted_coef = [];
for i = 0:(factor_l - 1),
  for j = 1:factor_l:(order * factor_l),
    sorted_coef(k) = coef(j + i);
    k = k + 1;
  end
end
coef = sorted_coef;

// Generate file name
file_path = output_path + "dsp16_resampling_fircoef_ord" + string(order) + "_" + string(fs_in_hz) + "_" + string(fs_out_hz);
if normalized == 1 then
  file_path = file_path + "_norm";
end
file_path = file_path + ".h";

// Generate variable name
var_name = "dsp16_resampling_fircoef_ord" + string(order) + "_" + string(fs_in_hz) + "_" + string(fs_out_hz);
if normalized == 1 then
  var_name = var_name + "_norm";
end

// Display useful info
disp("Filter coefficients used for the Atmel DSPLib re-sampling algorithm.")
disp("Input sampling frequency: " + string(fs_in_hz) + " Hz");
disp("Output sampling frequency: " + string(fs_out_hz) + " Hz");
disp("Re-sampling order: " + string(order));
disp("Actual FIR filter order: " + string(order * factor_l));
if normalized == 1 then
  disp("Is normalized? yes");
else
  disp("Is normalized? no");
end
disp("Size: " + string(order * factor_l * 2) + " bytes");
disp("Output: " + file_path);
disp("Variable name: " + var_name);

disp("Writing output file...");
// Write FIR coefficients to the file
fd = mopen(file_path, "wt");
[err, msg] = merror(fd);
if err == 0 then

  // Print Software Franework license.
  mputstr("/*! \file *********************************************************************" + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * \brief FIR coefficients for the dsp16_resampling function from the DPSLib." + EOL, fd);
  mputstr(" *        These coefficients have the following caracterizations:" + EOL, fd);
  mputstr(" *          - Input sampling frequency: " + string(fs_in_hz) + " Hz" + EOL, fd);
  mputstr(" *          - Output sampling frequency: " + string(fs_out_hz) + " Hz" + EOL, fd);
  mputstr(" *          - Re-sampling order: " + string(order) + EOL, fd);
  if normalized == 1 then
    mputstr(" *          - Coefficients are normalized" + EOL, fd);
  end
  mputstr(" *          - Memory foot print: " + string(order * factor_l * 2) + " bytes" + EOL, fd);
  mputstr(" *         It matches the following parameters:" + EOL, fd);
  mputstr(" *         (dsp16_resampling_options_t::dsp16_custom_filter)" + EOL, fd);
  mputstr(" *          - Cut-off frequency (fc_hz): " + string(fc_hz) + EOL, fd);
  mputstr(" *          - Sampling frequency (fs_hz): " + string(fs_in_hz * factor_l) + EOL, fd);
  mputstr(" *          - Actual FIR filter order (order): " + string(order * factor_l) + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32" + EOL, fd);
  mputstr(" * - Supported devices:  All AVR32 devices with a USART module can be used." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * \author               Atmel Corporation: http://www.atmel.com \n" + EOL, fd);
  mputstr(" *                       Support and FAQ: http://www.atmel.com/design-support/" + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" ******************************************************************************/" + EOL, fd);
  mputstr("" + EOL, fd);
  mputstr("/* Copyright (c) 2010-2014 Atmel Corporation. All rights reserved." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * Redistribution and use in source and binary forms, with or without" + EOL, fd);
  mputstr(" * modification, are permitted provided that the following conditions are met:" + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * 1. Redistributions of source code must retain the above copyright notice, this" + EOL, fd);
  mputstr(" * list of conditions and the following disclaimer." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * 2. Redistributions in binary form must reproduce the above copyright notice," + EOL, fd);
  mputstr(" * this list of conditions and the following disclaimer in the documentation" + EOL, fd);
  mputstr(" * and/or other materials provided with the distribution." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * 3. The name of Atmel may not be used to endorse or promote products derived" + EOL, fd);
  mputstr(" * from this software without specific prior written permission." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * 4. This software may only be redistributed and used in connection with an Atmel" + EOL, fd);
  mputstr(" * AVR product." + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" * THIS SOFTWARE IS PROVIDED BY ATMEL ""AS IS"" AND ANY EXPRESS OR IMPLIED" + EOL, fd);
  mputstr(" * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF" + EOL, fd);
  mputstr(" * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE" + EOL, fd);
  mputstr(" * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR" + EOL, fd);
  mputstr(" * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES" + EOL, fd);
  mputstr(" * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;" + EOL, fd);
  mputstr(" * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND" + EOL, fd);
  mputstr(" * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT" + EOL, fd);
  mputstr(" * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS" + EOL, fd);
  mputstr(" * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE" + EOL, fd);
  mputstr(" *" + EOL, fd);
  mputstr(" */" + EOL, fd);

  
  mputstr("A_ALIGNED static const dsp16_t " + var_name + "[] = {" + EOL, fd);
  format("v", 6);
  for i=1:length(coef),
    str = "";
    if i <> 1 then
      str = str + "," + EOL;
    end
    str = str + "  DSP16_Q(" + msprintf('%8.8f', coef(i)) + ")";
    mputstr(str, fd);
  end
  mputstr(EOL + "};" + EOL, fd);
  disp("Done.");
else
  disp("Error: " + string(err) + " " + msg);
end
mclose(fd);
