
bps_list =[9600, 38400, 115200, 125000] #BAUD_RATE
fcpu =8000000 #F_CPU

def baud_setting(fcpu, bps):
    return int( ((fcpu + bps * 4) / (bps * 8)) - 1 )
    
def baud_actual(fcpu, bps):
     return ( (fcpu/(8 * (baud_setting(fcpu, bps)+1))) )

def baud_error(fcpu, bps):
    return ( (( 100*(bps - baud_actual(fcpu, bps)) ) / bps) )
    
for bps in bps_list:
    print("BAUD_SETTING:%i  ACTUAL:%f  ERROR:%f" % (baud_setting(fcpu, bps), baud_actual(fcpu, bps), baud_error(fcpu, bps)))
input("Any key won't work you have to Enter to exit, a deep thought ;)")
