; $$$$$$$$$$$$$$$$$$$ ABAKIS $$$$$$$$$$$$$$$$$$$$$
; *************** STAR^2 SOFTWARE ****************
; ??????????????????? GPS.INC ????????????????????

; GPSAPI, WM 6.5

align void gps.h

numeric GPS_FIX_MODE_*,\
 UNKNOWN, MSA, MSB, MSS, COUNT

numeric GPS_FIX_SELECTION_*,\
 UNKNOWN, AUTO, MANUAL

numeric GPS_HW_STATE_*, UNKNOWN, ON, IDLE
numeric GPS_FIX_*, UNKNOWN, 2D, 3D
numeric GPS_FIX_QUALITY_*, UNKNOWN, GPS, DGPS

GPS_MAX_SATELLITES=12
GPS_SS=4*GPS_MAX_SATELLITES
GPS_MAX_PREFIX_NAME=16
GPS_MAX_FRIENDLY_NAME=64

macro SATELLITE [a] {
 forward text a(GPS_SS)
}

macro GPS.STATUS [a] {
 forward
  a: integer a#.valid	       ; valid field bits
  integer a#.hw.state	       ; hardware state
  integer a#.eph.mask	       ; ephemeris SV
  integer a#.alm.mask	       ; almanac SV
  SATELLITE \		       ; in view...
   a#.satellites.prns,\        ; PRN numbers
   a#.satellites.strength      ; signal strength
  integer a#.error
}

macro GPS.DEVICE [a] {
 forward
  a: integer a#.version        ; 1 for this
  integer a#.size	       ; of this structure
  integer a#.driver.state      ; IOCTL_SERVICE_STATUS
  integer a#.device.state      ; IOCTL_SERVICE_STATUS
  FILETIME a#.last.time        ; last data received
  text a#.driver.prefix(32)    ; to open hardware
  text a#.multiplex.prefix(32) ; to open multiplex
  text a#.friendly.name(128)   ; of hardware
  GPS.STATUS a#.status	       ; status
}

macro GPS.POSITION [a] {
 forward
  a: integer a#.version        ; 1 for this
  integer a#.size	       ; of this structure
  integer a#.valid	       ; GPS_VALID_FLAGS
  integer a#.flags	       ; GPS_DATA_FLAGS
  SYSTEMTIME a#.time	       ; satellite time
  double a#.latitude	       ; positive=N
  double a#.longitude	       ; positive=E
  float a#.speed	       ; in nautical miles
  float a#.heading	       ; in degrees. 0=N
  double a#.magnetic.v	       ; magnetic variation
  float a#.altitude.sea        ; altitude in meters
  float a#.altitude.ellipsoid  ; of sea level. WGS84
  integer a#.fix.quality       ; quality
  integer a#.fix.type	       ; 2D or 3D?
  integer a#.fix.select        ; auto/manual 2D/3D?
  float a#.precision	       ; accuracy, pdop
  float a#.hprecision	       ; hdop
  float a#.vprecision	       ; vdop
  integer a#.satellites.used   ; # satellites used
  SATELLITE a#.satellites.prn  ; PRN numbers used
  integer a#.satellites.view   ; # satellites
  SATELLITE \		       ; in view...
   a#.satellites.prns,\        ; PRN numbers
   a#.satellites.elevation,\   ; elevations. degrees
   a#.satellites.azimuth,\     ; azimuth angles
   a#.satellites.strength      ; signal strengths
}

purge SATELLITE

; current GPS information

align

GPS.DEVICE gps.device
GPS.STATUS gps.status
GPS.POSITION gps.position

integer gps.error

; initialize gps driver. parameters are event
; handles (CreateEvent) for when new GPS data
; is available or a change in the state occurs.
; they may be 0

macro open.gps new.h, change.h {
 get gps.h=GPSOpenDevice new.h, change.h, 0
}

; get GPS information. returns 0 if failure
; and gps.error contains the code/reason

AGE.MS=10*1000 ; maximum age in ms

macro get.gps.position {
  get gps.error=GPSGetPosition gps.h,\
   gps.position, AGE.MS
  if true, r0=0, else, r0=1, end
}

; get GPS device state

macro get.gps.state {
  get gps.error=GPSGetDeviceState gps.device
  if true, r0=0, else, r0=1, end
}