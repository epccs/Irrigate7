
bps =115200 #BAUD_RATE
fcpu =16000000 #F_CPU note that 18.432MHz is the correct freq for baud rate
BAUD_SETTING= int( ((fcpu + bps * 4) / (bps * 8)) - 1 ) 
BAUD_ACTUAL=( (fcpu/(8 * (BAUD_SETTING+1))) )
BAUD_ERROR=( (( 100*(bps - BAUD_ACTUAL) ) / bps) )
print("BAUD_SETTING:%i  ACTUAL:%f  ERROR:%f" % (BAUD_SETTING, BAUD_ACTUAL, BAUD_ERROR))
input()
