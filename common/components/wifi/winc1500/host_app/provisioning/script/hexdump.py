path_img  = "logo.png"
path_html = "provisioning.html"
path_output = "html_logo_c_array.txt"
img_file  = open(path_img, "rb")
html_file = open(path_html, "rb")
img_read  = img_file.read()
html_read = html_file.read()
img_file.close()
html_file.close()

i_length = 0
h_length = 0
img_val = ""
html_val =""
for x in img_read:
    img_val += "0x%02x, ~" % ord(x)
    i_length += 1

for x in html_read:
    html_val += "0x%02x, ~" % ord(x)
    h_length += 1

logo_response_header = "HTTP/1.0 200 OK\r\nContent-type: image/png\r\naccept-ranges: bytes\r\nstatus: 200\r\nContent-Length: "+str(i_length)+"\r\n\r\n"
html_response_header = "HTTP/1.0 200 OK\r\nContent-type: text/html; charset=utf-8\r\ncache-control: private, no-cache, no-store, must-revalidate\r\nConnection: Keep-Alive\r\nKeep-Alive: timeout=1200, max=100\r\nX-XSS-Protection: 0\r\nContent-Length: "+str(h_length)+"\r\n\r\n"

logo_header_val = ""
html_header_val = ""
for y in logo_response_header:
    logo_header_val += "0x%02x, ~" % ord(y)
for y in html_response_header:
    html_header_val += "0x%02x, ~" % ord(y)

#format the data for printing
logo_buf_length = 0
html_buf_length = 0
logo_temp = logo_header_val + img_val
html_temp = html_header_val + html_val

logo_val = logo_temp.split('~')
html_val = html_temp.split('~')
logo_response_data = " \t"
html_response_data = " \t"

for z in logo_val:
    logo_response_data += z
    logo_buf_length += 1
    if (logo_buf_length %15) == 0:
        logo_response_data += '\n\t'    
for z in html_val:
    html_response_data += z
    html_buf_length += 1
    if (html_buf_length %15) == 0:
        html_response_data += '\n\t'    

logo_buff = "const uint8_t logo_buff["+str(logo_buf_length)+"] = {\n" + logo_response_data + "\n};\r\n";
html_buff = "const uint8_t html_buff["+str(html_buf_length)+"] = {\n" + html_response_data + "\n};\r\n";

output_file = open(path_output, 'w')
output_file.write(html_buff)
output_file.write("\r\n\n")
output_file.write(logo_buff)
output_file.close()
print "The Hex values of %s and %s is generated in %s"%(path_html,path_img,path_output)
print "Copy paste the generated coefficient in html_page_buf.h"
