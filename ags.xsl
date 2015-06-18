<?xml version="1.0"?>
<!DOCTYPE xsl[
<!ENTITY version "0.4.2">
<!ENTITY buildid "XSLT">
]>

<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- ticks to tact -->
  <xsl:template name="ticks-to-tact">
    <xsl:param name="current-tick"/>

    <xsl:variable name="division" select="//midi-header/@division"/>
    <xsl:variable name="tempo" select="//midi-tracks/midi-track[1]/midi-message[@event='tempo-number']/@tempo"/>
    <xsl:variable name="secs">60.0</xsl:variable>
    <xsl:variable name="microsecs">1000000.0</xsl:variable>
    
    <xsl:if test="${division} &gt; 0">
      
      <xsl:value-of select="${secs} div ${current-tick} div ${tempo} * ${division} * ${microsecs}"/>
    </xsl:if>

    <xsl:if test="not(${division} &gt; 0)">
      <xsl:variable name="smtpe-format">floor(${division} div 256)</xsl:variable>
      <xsl:variable name="smtpe-resoulution">floor(${division} div 256) * 256</xsl:variable>

      <xsl:value-of select="${secs} div ${current-tick} * ${smtpe-format} * ${smtpe-resolution} * ${microsecs}"/>
    </xsl:if>
  </xsl:template>

  
  <!-- midi read bpm -->
  <xsl:template name="midi-read-bpm">
    <xsl:variable name="ticks" select="//midi-header/@division"/>
    <xsl:variable name="division" select="//midi-header/@division"/>
    <xsl:variable name="tempo" select="//midi-tracks/midi-track[1]/midi-message[@event='tempo-number']/@tempo"/>
    <xsl:variable name="secs">60.0</xsl:variable>
    <xsl:variable name="microsecs">1000000.0</xsl:variable>
    
    <xsl:if test="${division} &gt; 0">

      <xsl:value-of select="${secs} div ${ticks} div ${tempo} * ${division} * ${microsecs}"/>

    </xsl:if>

    <xsl:if test="not(${division} &gt; 0)">
      <xsl:variable name="smtpe-format">floor(${division} div 256)</xsl:variable>
      <xsl:variable name="smtpe-resoulution">floor(${division} div 256) * 256</xsl:variable>

      <xsl:value-of select="${secs} div ${ticks} * ${smtpe-format} * ${smtpe-resolution} * ${microsecs}"/>

    </xsl:if>
  </xsl:template>
  
  <!-- note node -->
  <xsl:template name="add-note">
    <xsl:variable name="sequence" select="../midi-message[boolean(@sequence-name)]/@sequence-name"/>
    <xsl:variable name="instrument" select="../midi-message[boolean(@instrument-name)]/@instrument-name"/>
    <xsl:variable name="current-tick"><xsl:value-of select="./@delta-time"></xsl:variable>

    <xsl:variable name="current-note">
      <ags-note>
	
	<xsl:attribute name="id">{generate-id(ags-note)}</xsl:attribute>
	<xsl:attribute name="flags">0</xsl:attribute>

	<xsl:variable name="y" select="./@note"/>
	<xsl:attribute name="y"><xsl:value-of select="${y}"/></xsl:attribute>
	
	<xsl:variable name="x0">
	  <xsl:call-template name="ticks-to-tact">
	    <xsl:with-param name="current-tick" select="${current-tick}"/>
	  </xsl:call-template>
	</xsl:variable>
	<xsl:attribute name="x0"><xsl:value-of select="${x0}"/></xsl:attribute>
	
	<xsl:if test="following-sibling::[@event='note-off' and @note='${y}']">
	  <xsl:variable name="next-tick"><xsl:value-of select="following-sibling::[@event='note-off' and @note='${y}]/@delta-time"></xsl:variable>

	  <xsl:variable name="x1">
	    <xsl:call-template name="ticks-to-tact">
	      <xsl:with-param name="current-tick" select="${next-tick}"/>
	    </xsl:call-template>
	  </xsl:variable>
	  
	  <xsl:attribute name="x1">
	    <xsl:value-of select="${x1}"/>
	  </xsl:attribute>
	</xsl:if>
	
	<xsl:if test="not(following-sibling::[@event='note-off' and @note='${y}'])">
	  <xsl:variable name="add-one" select="1"/>
	  <xsl:attribute name="x1"><xsl:value-of select="${x0} + ${add-one}"/></xsl:attribute>
	</xsl:if>
	
      </ags-note>
    </xsl:variable>
    
  </xsl:template>
  
  <!-- audio node -->
  <xsl:template name="add-audio">
    <xsl:param name="sequence"/>
    <xsl:param name="instrument"/>
    
    <xsl:variable name="current-audio">
      <ags-audio>
	<xsl:attribute name="id">{generate-id(ags-audio)}</xsl:attribute>
	<xsl:attribute name="flags">0</xsl:attribute>
	<xsl:attribute name="sequence-length">0</xsl:attribute>
	<xsl:attribute name="audio-channels">2</xsl:attribute>
	<xsl:attribute name="output-pads">1</xsl:attribute>
	<xsl:attribute name="input-pads">78</xsl:attribute>
	<xsl:attribute name="devout">xpath=../../</xsl:attribute>

	<ags-notation-list>
	  <xsl:attribute name="id">{generate-id(ags-notation-list)}</xsl:attribute>

	  <ags-notation>
	    <xsl:attribute name="id">{generate-id(ags-notation)}</xsl:attribute>
	    
	    <ags-note-list>
	      <xsl:attribute name="id">{generate-id(ags-note-list)}</xsl:attribute>

	      <xsl:for-each select="document('')/*/xsl:variable[@name='current-note']/*">

		<xsl:if test="current()/xsl:variable[@name='sequence']=${sequence} and current()/xsl:variable[@name='instrument']=:${instrument}']">
		  <xsl:value-of select="current()"/>
		</xsl:if>
		
	      </xsl:for-each>
	    </ags-note-list>
	  </ags-notation>
	</ags-notation-list>
      </ags-audio>
    </xsl:variable>

    <xsl:value-of select="${current-audio}/@id"/>
    
  </xsl:template>
  
  <!-- machine node -->
  <xsl:template name="add-machine">
    <xsl:param name="audio"/>

    <xsl:variable name="current-machine">
      <ags-machine>
	<xsl:attribute name="type">AgsFFPlayer</xsl:attribute>
	<xsl:attribute name="id">{generate-id(ags-machine)}</xsl:attribute>
	<xsl:attribute name="version">&version;</xsl:attribute>
	<xsl:attribute name="build-id">&buildid;</xsl:attribute>
	<xsl:attribute name="flags">0</xsl:attribute>
	<xsl:attribute name="file-input-flags">0</xsl:attribute>
	<xsl:attribute name="name">default </xsl:attribute>
	<xsl:attribute name="audio">xpath=//ags-audio[@id='${audio}']</xsl:attribute>

	<ags-ffplayer>
	  <xsl:attribute name="id">{generate-id(ags-ffplayer)}</xsl:attribute>
	  <xsl:attribute name="filename"></xsl:attribute>
	  <xsl:attribute name="preset"></xsl:attribute>
	  <xsl:attribute name="instrument"></xsl:attribute>
	</ags-ffplayer>
      </ags-machine>
    </xsl:variable>

  </xsl:template>

  <!-- note on -->
  <xsl:template match="midi-message[@event='note-on']">
    <xsl:call-template name="add-note"/>
  </xsl:template>

  <!-- midi track -->
  <xsl:template match="midi-track">
    <xsl:variable name="sequence" select="./midi-message[boolean(@sequence-name)]/@sequence-name"/>
    <xsl:variable name="instrument" select="./midi-message[boolean(@instrument-name)]/@instrument-name"/>

    <xsl:if test="not(preceding-sibling::midi-message[boolean(@sequence-name)] == ${sequence} and preceding-sibling::midi-message[boolean(@instrument-name)] == ${instrument})">
      <xsl:variable name="current-audio">
	<xsl:call-template name="add-audio">
	  <xsl:with-param name="sequence" select="${sequence}"/>
	  <xsl:with-param name="instrument" select="${instrument}"/>
	</xsl:call-template>
      </xsl:variable>
      
      <xsl:call-template name="add-machine">
	<xsl:with-param name="audio" select="${current-audio}"/>
      </xsl:call-template>
    </xsl:if>

    <xsl:apply-templates/>
    
  </xsl:template>

  <xsl:template match="midi-header">
    <xsl:call-template name="midi-read-bpm"/>
  </xsl:template>

  <xsl:template match="midi-tracks">
    <xsl:apply-templates/>
  </xsl:template>
  
  <!-- root node -->
  <xsl:template match="/">    
    <ags>
      <ags-main version="&version;" build-id="&buildid;">
	<xsl:attribute name="id">{generate-id(ags-main)}</xsl:attribute>

	<ags-devout-list>
	  <xsl:attribute name="id">{generate-id(ags-devout-list)}</xsl:attribute>
	  
	  <ags-devout>
	    <xsl:attribute name="id">{generate-id(ags-devout)}</xsl:attribute>
	    <xsl:attribute name="flags">0</xsl:attribute>
	    <xsl:attribute name="bpm">
	      <xsl:call-template name="midi-read-bpm"/>
	    </xsl:attribute>
	    
	    <ags-audio-list>
	      <xsl:attribute name="id">{generate-id(ags-audio-list)}</xsl:attribute>

	      <xsl:copy-of select="document('')/*/xsl:variable[@name='current-audio']/*"/>
	      
	    </ags-audio-list>
	  </ags-devout>
	</ags-devout-list>
	<ags-window>
	  <xsl:attribute name="id">{generate-id(ags-window)}</xsl:attribute>

	  <ags-machine-list>
	    <xsl:attribute name="id">{generate-id(ags-machine-list)}</xsl:attribute>

	    <xsl:copy-of select="document('')/*/xsl:variable[@name='current-machine']/*"/>
    	    
	  </ags-machine-list>
	</ags-window>
      </ags-main>
    </ags>

    <xsl:apply-templates/>

  </xsl:template>

</xsl:stylesheet> 
