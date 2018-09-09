import sys

X509_HEADER = "-----BEGIN CERTIFICATE-----\r\n"
X509_FOOTER = "\r\n-----END CERTIFICATE-----\r\n"
PEM_HEADER = "-----BEGIN CERTIFICATE-----\n"
PEM_FOOTER = "\n-----END CERTIFICATE-----\n"

try:
    import base64
except ImportError as e:
    print e
    print "Please install base64"
    sys.exit()
except Exception as e:
    print 'An error occured.'
    print e    
    sys.exit()

try:
    from Crypto.PublicKey import RSA
except ImportError as e:
    print e
    print "This script require pycrypto and it is not installed"
    print "Please refer https://pypi.org/project/pycrypto for details"
    sys.exit()
except Exception as e:
    print 'An error occured.'
    print e    
    sys.exit()
    

path_rsa_key_file = "demo_rsa.key"
path_rsa_cert_file = "demo_rsa.crt"

try:
    rsakey_file = open(path_rsa_key_file, "rb")
    cert_file =  open(path_rsa_cert_file, "rb")
except Exception as e:
    print e
    sys.exit()
    
try:
    rsa_key  = RSA.importKey(rsakey_file.read())
    rsakey_file.close()
    cert_read = cert_file.read()
    cert_file.close()
except Exception as e:
    print 'An error occured.'
    print e    
    sys.exit()

m_length = 0
modulus_hex = bytearray.fromhex('{:0192x}'.format(rsa_key.n))

modulus_val = "\t"
for i in modulus_hex:
    modulus_val += "0x%0.2X, " % i
    m_length += 1
    if (m_length % 15) == 0:
        modulus_val += '\n\t'
modulus = "const uint8_t modulus["+str(m_length)+"] = {\n" + modulus_val + "\n};\r\n\n";

p_length = 0
private_key_val = "\t"
private_key_hex = bytearray.fromhex('{:0192x}'.format(rsa_key.d))
for j in private_key_hex:
    private_key_val += "0x%0.2X, " % j
    p_length += 1
    if (p_length % 15) == 0:
        private_key_val += '\n\t'
private_key = "const uint8_t exponent["+str(p_length)+"] = {\n" + private_key_val + "\n};\r\n\n";

c_length = 0
cert_val = "\t"

if ((cert_read.find(X509_HEADER)==0) and (cert_read.find(X509_FOOTER)>0)):
    a,b = cert_read.split(X509_HEADER)
    c,d = b.split(X509_FOOTER)
    cert_read =base64.b64decode(c)
elif ((cert_read.find(PEM_HEADER)==0) and (cert_read.find(PEM_FOOTER)>0)):
    a,b = cert_read.split(PEM_HEADER)
    c,d = b.split(PEM_FOOTER)
    cert_read =base64.b64decode(c)

for k in cert_read:
    cert_val += "0x%02x, " % ord(k)
    c_length +=1
    if(c_length % 15) == 0:
        cert_val += '\n\t'
certificate = "const uint8_t certificate["+str(c_length)+"] = {\n" + cert_val + "\n};\r\n\n";
        
output_file = open("privateKey_decoded.txt", 'w')
output_file.write(modulus)
output_file.write(private_key)
output_file.write(certificate)
output_file.close()

print modulus
print private_key
print certificate
