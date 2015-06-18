<?xml version="1.0"?>
<!DOCTYPE xsl[
<!ENTITY version "0.4.2">
<!ENTITY buildid "XSLT">
]>

<xsl:stylesheet version="1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- note node -->
  <xsl:template match="midi-message[@event='note-on']">
    <xsl:param name="audio"/>
    
    <xsl:copy>
      <xsl:apply-templates select="@* | node(//ags-audio[@id='${audio}'])/ags-notation-list/ags-notation/"/>
    </xsl:copy>

    <ags-note>
      <xsl:attribute name="id">{generate-id(ags-note)}</xsl:attribute>
      <xsl:attribute name="flags">0</xsl:attribute>

      <xsl:attribute name="y"><xsl:value-of select="./@note"/></xsl:attribute>
      <xsl:variable name="y" select="./@note"/>
      
      <xsl:attribute name="x0"><xsl:value-of select="./@delta-time"/></xsl:attribute>
      
      <xsl:if test="../midi-message[@event='note-off' and @note='${y}']">
	<xsl:attribute name="x1"><xsl:value-of select="../midi-message[@event='note-off' and @note='${y}]/@delta-time"/></xsl:attribute>
      </xsl:if>
      <xsl:if test="not(../midi-message[@event='note-off' and @note='${y}'])">
	<xsl:variable name="add-one" select="1"/>
	<xsl:attribute name="x1"><xsl:value-of select="${y} + ${add-one}"/></xsl:attribute>
      </xsl:if>
      
    </ags-note>
  </xsl:template>
  
  <!-- audio node -->
  <xsl:template name="add-audio">
    <xsl:copy>
      <xsl:apply-templates select="@* | node(/ags/ags-main/ags-devout-list/ags-devout/ags-audio-list)"/>
    </xsl:copy>

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

	    <!-- to be filled by xsl:template[name="add-note"] -->
	    
	  </ags-note-list>
	</ags-notation>
      </ags-notation-list>
    </ags-audio>

    <xsl:call-template name="add-machine">
      <xsl:with-param name="audio-id">
	<xsl:variable name="audio" select="audio/@id"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>
  
  <!-- machine node -->
  <xsl:template name="add-machine">
    <xsl:param name="audio"/>
    
    <xsl:copy>
      <xsl:apply-templates select="@* | /ags/ags-main/ags-window/ags-machine-list"/>
    </xsl:copy>
    
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

	    <ags-audio-list>
	      <xsl:attribute name="id">{generate-id(ags-audio-list)}</xsl:attribute>
		    
	      <!-- to be filled by xsl:template[name="add-audio"] -->
	      
	    </ags-audio-list>
	  </ags-devout>
	</ags-devout-list>
	<ags-window>
	  <xsl:attribute name="id">{generate-id(ags-window)}</xsl:attribute>

	  <ags-machine-list>
	    <xsl:attribute name="id">{generate-id(ags-machine-list)}</xsl:attribute>

	    <!-- to be filled by xsl:template[name="add-machine"] -->
	    
	  </ags-machine-list>
	</ags-window>
      </ags-main>
    </ags>
  </xsl:template>

</xsl:stylesheet> 
