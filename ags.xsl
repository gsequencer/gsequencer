<?xml version="1.0"?>
<!DOCTYPE xsl[
<!ENTITY version "0.4.2">
<!ENTITY buildid "XSLT">
<!ENTITY audiochannels "1">
<!ENTITY defaultsf2 ""> <!-- /usr/share/sounds/sf2/FluidR3_GM.sf2 -->
<!ENTITY preset ""> <!-- "Yamaha Grand Piano" -->
<!ENTITY instrument ""> <!-- "Yamaha Grand Piano" -->
]>

<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" indent="yes" omit-xml-declaration="no"/>
  <xsl:strip-space elements="*"/>
  
  <!-- ticks to tact -->
  <xsl:template name="ticks-to-tact">
    <xsl:param name="current-tick"/>

    <xsl:variable name="division" select="//midi-header/@division"/>
    <xsl:variable name="tempo" select="//midi-tracks/midi-track[1]/midi-message[@event='tempo-number']/@tempo"/>
    <xsl:variable name="secs">60.0</xsl:variable>
    <xsl:variable name="microsecs">1000000.0</xsl:variable>

    <xsl:variable name="bpm">
      <xsl:call-template name="midi-read-bpm"/>
    </xsl:variable>

    <xsl:if test="$division &gt; 0">
      
      <xsl:value-of select="16.0 * $bpm div $secs * $current-tick * $tempo div $division div $microsecs"/>
    </xsl:if>

    <xsl:if test="not($division &gt; 0)">
      <xsl:variable name="smtpe-format">floor($division div 256)</xsl:variable>
      <xsl:variable name="smtpe-resoulution">floor($division div 256) * 256</xsl:variable>

      <xsl:value-of select="16.0 * $bpm div $secs * $current-tick div $smtpe-format div $smtpe-resolution div $microsecs"/>
    </xsl:if>
  </xsl:template>

  
  <!-- midi read bpm -->
  <xsl:template name="midi-read-bpm">
    <xsl:variable name="ticks" select="//midi-header/@division"/>
    <xsl:variable name="division" select="//midi-header/@division"/>
    <xsl:variable name="tempo" select="//midi-tracks/midi-track[1]/midi-message[@event='tempo-number']/@tempo"/>
    <xsl:variable name="secs">60.0</xsl:variable>
    <xsl:variable name="microsecs">1000000.0</xsl:variable>
    
    <xsl:if test="$division &gt; 0">

      <xsl:value-of select="$secs div $ticks div $tempo * $division * $microsecs"/>

    </xsl:if>

    <xsl:if test="not($division &gt; 0)">
      <xsl:variable name="smtpe-format">floor($division div 256)</xsl:variable>
      <xsl:variable name="smtpe-resoulution">floor($division div 256) * 256</xsl:variable>

      <xsl:value-of select="$secs div $ticks * $smtpe-format * $smtpe-resolution * $microsecs"/>

    </xsl:if>
  </xsl:template>
  
  <!-- note node -->
  <xsl:template match="midi-message">
    <xsl:param name="parent-sequence"/>
    <xsl:param name="parent-instrument"/>

    <xsl:if test="current()/@event='note-on'">

      <xsl:variable name="sequence" select="../midi-message[boolean(@sequence-name)]/@sequence-name"/>
      <xsl:variable name="instrument" select="../midi-message[boolean(@instrument-name)]/@instrument-name"/>
      <xsl:variable name="current-tick"><xsl:value-of select="./@delta-time"/></xsl:variable>

      <xsl:if test="$parent-sequence=$sequence and $parent-instrument=$instrument">
	<ags-note>
	  
	  <xsl:attribute name="id">note-<xsl:value-of select="position()"/></xsl:attribute>
	  <xsl:attribute name="flags">0</xsl:attribute>

	  <xsl:variable name="y" select="./@note"/>
	  <xsl:attribute name="y"><xsl:value-of select="$y"/></xsl:attribute>
	  
	  <xsl:variable name="x0">
	    <xsl:call-template name="ticks-to-tact">
	      <xsl:with-param name="current-tick" select="$current-tick"/>
	    </xsl:call-template>
	  </xsl:variable>
	  <xsl:attribute name="x0"><xsl:value-of select="round($x0 div 16.0)"/></xsl:attribute>
	  
	  <xsl:if test="following-sibling::node()[@event='note-off' and @note='$y']">
	    <xsl:variable name="next-tick"><xsl:value-of select="following-sibling::node()[@event='note-off' and @note='$y']/@delta-time"/></xsl:variable>

	    <xsl:variable name="x1">
	      <xsl:call-template name="ticks-to-tact">
		<xsl:with-param name="current-tick" select="$next-tick"/>
	      </xsl:call-template>
	    </xsl:variable>
	    
	    <xsl:attribute name="x1">
	      <xsl:value-of select="floor($x1)"/>
	    </xsl:attribute>
	  </xsl:if>
	  
	  <xsl:if test="not(following-sibling::ags-note[@event='note-off' and @note='$y'])">
	    <xsl:variable name="add-one" select="1"/>
	    <xsl:attribute name="x1"><xsl:value-of select="round($x0 div 16.0) + $add-one"/></xsl:attribute>
	  </xsl:if>
	  
	</ags-note>
      </xsl:if>
      
    </xsl:if>
    
  </xsl:template>

  <!-- notation node -->
  <xsl:template name="add-notation">
    <xsl:param name="sequence"/>
    <xsl:param name="instrument"/>
    <xsl:param name="num" select="1"/>
    <xsl:param name="count"/>

    <ags-notation>
      <xsl:attribute name="id">notation-<xsl:value-of select="position()"/></xsl:attribute>
      <xsl:attribute name="flags">0</xsl:attribute>
      <xsl:attribute name="audio-channel"><xsl:value-of select="$num - 1"/></xsl:attribute>
      <xsl:attribute name="base-note">C</xsl:attribute>
      
      <ags-note-list>
	<xsl:attribute name="id">note-list-<xsl:value-of select="position()"/></xsl:attribute>

	<xsl:apply-templates select="/midi/midi-tracks/midi-track/midi-message">
	  <xsl:with-param name="parent-sequence" select="$sequence"/>
	  <xsl:with-param name="parent-instrument" select="$instrument"/>
	</xsl:apply-templates>

      </ags-note-list>
    </ags-notation>
    
    <xsl:if test="$num &lt; $count">
      <xsl:call-template name="add-notation">
	<xsl:with-param name="num" select="$num + 1"/>
	<xsl:with-param name="count" select="$count"/>
      </xsl:call-template>
    </xsl:if>
    
  </xsl:template>
  
  <!-- audio node -->
  <xsl:template name="add-audio">
    <xsl:param name="sequence"/>
    <xsl:param name="instrument"/>
    
    <ags-audio>
      <xsl:attribute name="id"><xsl:value-of select="$sequence"/>_<xsl:value-of select="$instrument"/></xsl:attribute>
      <xsl:attribute name="flags">13f</xsl:attribute>
      <xsl:attribute name="sequence-length">0</xsl:attribute>
      <xsl:attribute name="audio-channels">&audiochannels;</xsl:attribute>
      <xsl:attribute name="output-pads">1</xsl:attribute>
      <xsl:attribute name="input-pads">128</xsl:attribute>
      <xsl:attribute name="devout">xpath=//ags-devout</xsl:attribute>
      
      <ags-notation-list>
	<xsl:attribute name="id">notation-list-<xsl:value-of select="position()"/></xsl:attribute>

	<xsl:call-template name="add-notation">
	  <xsl:with-param name="count" select="&audiochannels;"/>
	  <xsl:with-param name="sequence" select="$sequence"/>
	  <xsl:with-param name="instrument" select="$instrument"/>
	</xsl:call-template>
	
      </ags-notation-list>
    </ags-audio>
    
  </xsl:template>
  
  <!-- machine node -->
  <xsl:template name="add-machine">
    <xsl:param name="sequence"/>
    <xsl:param name="instrument"/>

    <ags-machine>
      <xsl:attribute name="type">AgsFFPlayer</xsl:attribute>
      <xsl:attribute name="id">machine-<xsl:value-of select="position()"/></xsl:attribute>
      <xsl:attribute name="flags">628</xsl:attribute>
      <xsl:attribute name="version">&version;</xsl:attribute>
      <xsl:attribute name="build-id">&buildid;</xsl:attribute>
      <xsl:attribute name="file-input-flags">0</xsl:attribute>
      <xsl:attribute name="name"><xsl:value-of select="$sequence"/> - <xsl:value-of select="$instrument"/></xsl:attribute>
      <xsl:attribute name="audio">xpath=//ags-audio[@id='<xsl:value-of select="$sequence"/>_<xsl:value-of select="$instrument"/>']</xsl:attribute>

      <ags-ffplayer>
	<xsl:attribute name="id">ffplayer-<xsl:value-of select="position()"/></xsl:attribute>
	<xsl:attribute name="filename">&defaultsf2;</xsl:attribute>
	<xsl:attribute name="preset">&preset;</xsl:attribute>
	<xsl:attribute name="instrument">&instrument;</xsl:attribute>
      </ags-ffplayer>
    </ags-machine>

  </xsl:template>

  <!-- midi track -->
  <xsl:template match="/midi/midi-tracks/midi-track">
    <xsl:param name="parent"/>

    <xsl:if test="not(midi-message[@event='time-signature'])">
      <xsl:variable name="sequence" select="midi-message[boolean(@sequence-name)]/@sequence-name"/>
      <xsl:variable name="instrument" select="midi-message[boolean(@instrument-name)]/@instrument-name"/>

      <xsl:variable name="previous" select="preceding-sibling::node()"/>

      <xsl:if test="not($previous/midi-message/@sequence-name=$sequence and $previous/midi-message/@instrument-name=$instrument)">
	<xsl:if test="$parent='ags-audio-list'">
	  <xsl:call-template name="add-audio">
	    <xsl:with-param name="sequence" select="$sequence"/>
	    <xsl:with-param name="instrument" select="$instrument"/>
	  </xsl:call-template>
	</xsl:if>
	
	<xsl:if test="$parent='ags-machine-list'">
	  <xsl:call-template name="add-machine">
	    <xsl:with-param name="sequence" select="$sequence"/>
	    <xsl:with-param name="instrument" select="$instrument"/>
	  </xsl:call-template>
	</xsl:if>	
      </xsl:if>
    </xsl:if>    
  </xsl:template>

  <xsl:template match="midi-header">    
    <xsl:variable name="bpm">
      <xsl:call-template name="midi-read-bpm"/>
    </xsl:variable>
  </xsl:template>

  <!-- root node -->
  <xsl:template match="/">    
    <ags>
      <ags-main version="&version;" build-id="&buildid;" flags="0">
	<xsl:attribute name="id">ags-main-<xsl:value-of select="position()"/></xsl:attribute>

	<ags-devout-list>
	  <xsl:attribute name="id">devout-list-<xsl:value-of select="position()"/></xsl:attribute>
	  
	  <ags-devout>
	    <xsl:attribute name="id">ags-devout-<xsl:value-of select="position()"/></xsl:attribute>
	    <xsl:attribute name="flags">100</xsl:attribute>
	    <xsl:attribute name="bpm">
	      <xsl:call-template name="midi-read-bpm"/>
	    </xsl:attribute>
	    
	    <ags-audio-list>
	      <xsl:attribute name="id">ags-audio-list-<xsl:value-of select="position()"/></xsl:attribute>

	      <xsl:apply-templates select="/midi/midi-tracks/midi-track">
		<xsl:with-param name="parent">ags-audio-list</xsl:with-param>
	      </xsl:apply-templates>
	      
	    </ags-audio-list>
	  </ags-devout>
	</ags-devout-list>
	<ags-window>
	  <xsl:attribute name="id">window-<xsl:value-of select="position()"/></xsl:attribute>
	  <xsl:attribute name="flags">0</xsl:attribute>
	  <xsl:attribute name="devout">xpath=//ags-devout</xsl:attribute>

	  <ags-machine-list>
	    <xsl:attribute name="id">machine-list-<xsl:value-of select="position()"/></xsl:attribute>

	    <xsl:apply-templates select="/midi/midi-tracks/midi-track">
	      <xsl:with-param name="parent">ags-machine-list</xsl:with-param>
	    </xsl:apply-templates>
    	    
	  </ags-machine-list>
	</ags-window>
      </ags-main>
    </ags>

  </xsl:template>

</xsl:stylesheet> 
