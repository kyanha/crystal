<?xml version="1.0"?>
<!--
    Copyright (C) 2002 by Norman Kr�mer
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">


  <xsl:output method="text" indent="no"/>
  <xsl:strip-space elements="*"/>

  <xsl:template match="/UI">

    <xsl:choose>
      <xsl:when test="./class">
        <xsl:text>window "</xsl:text><xsl:value-of select="./class"/><xsl:text>"</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>window "</xsl:text><xsl:value-of select="./widget/property[name[text()='name']]/cstring"/><xsl:text>"</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates />

  </xsl:template>

  <xsl:template match="widget[class[text()='QDialog']]">
    <xsl:text>{</xsl:text>
    <!-- set the title if any -->
    <xsl:call-template name="style"/>
    <xsl:call-template name="options"/>
    <xsl:apply-templates select="widget"/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="widget[class[text()='QPushButton']]">
    <xsl:call-template name="component_header"/>
    <xsl:text>{</xsl:text>
    <xsl:call-template name="style"/>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>

  <xsl:template match="widget[class[text()='QRadioButton' or text()='QButtonGroup' or text()='QGroupBox' or text()='QCheckBox' or text()='QLineEdit' or text()='QLabel']]">
    <xsl:call-template name="component_header"/>
    <xsl:text>{</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
  </xsl:template>
  
  <xsl:template match="property[name[text()='caption']]">
    <xsl:text>Title: "</xsl:text><xsl:value-of select="string"/><xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="property[name[text()='text' or text()='title'] and ../class[text() != 'QLineEdit']]">
    <xsl:text>Caption: "</xsl:text><xsl:value-of select="string"/><xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="property[name[text()='text'] and ../class[text()='QLineEdit']]">
    <xsl:text>Text: "</xsl:text><xsl:value-of select="string"/><xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="property[name[text()='pixmap'] and ../class[text()='QLabel']]">
    <xsl:text>Texture: "</xsl:text><xsl:value-of select="string"/><xsl:text>"</xsl:text>
  </xsl:template>

  <xsl:template match="property[name[text()='geometry']]">
    <xsl:text>Frame: (</xsl:text><xsl:value-of select="rect/x"/><xsl:text>,</xsl:text><xsl:value-of select="rect/y"/><xsl:text>) - (</xsl:text><xsl:value-of select="number(rect/x)+number(rect/width)"/><xsl:text>,</xsl:text><xsl:value-of select="number(rect/y)+number(rect/height)"/><xsl:text>)</xsl:text>
  </xsl:template>

  <xsl:template match="property[name[text()='echoMode']]">
    <xsl:choose>
      <xsl:when test="enum='NoEcho'">
        <xsl:text>Masked: Yes</xsl:text>
      </xsl:when>
      <xsl:when test="enum='Password'">
        <xsl:text>Masked: Yes</xsl:text>
        <xsl:text>MaskChar: "*"</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="property[name[text()='frameShadow']]">
    <xsl:text>Style: </xsl:text>
    <xsl:choose>
      <xsl:when test="enum='Raised'">
        <xsl:call-template name="prefix"/><xsl:text>fsRaised</xsl:text>
      </xsl:when>
      <xsl:when test="enum='Plain'">
        <xsl:call-template name="prefix"/><xsl:text>fsSimple</xsl:text>
      </xsl:when>
      <xsl:when test="enum='Sunken'">
        <xsl:call-template name="prefix"/><xsl:text>fsSunken</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="property[name[text()='alignment']]">
    <xsl:text>Align: </xsl:text>
    <xsl:choose>
      <xsl:when test="contains(set,'AlignCenter')">
        <xsl:call-template name="prefix"/><xsl:text>AlignCenter</xsl:text>
      </xsl:when>
      <xsl:when test="contains(set,'AlignLeft')">
        <xsl:call-template name="prefix"/><xsl:text>AlignLeft</xsl:text>
      </xsl:when>
      <xsl:when test="contains(set,'AlignRight')">
        <xsl:call-template name="prefix"/><xsl:text>AlignRight</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
    
  <xsl:template name="style">
    <xsl:text>Style: </xsl:text><xsl:call-template name="prefix"/><xsl:text>fsNormal</xsl:text>
  </xsl:template>

  <xsl:template name="options">
    <xsl:choose>
      <xsl:when test="class[text()='QDialog']">
        <xsl:text>Options: wfoBeveledBorder+wfoGrip+wfoTitle+wfoClose+wfoMin+wfoZoom+wfoControl</xsl:text>
      </xsl:when>
      <xsl:when test="class[text()='QWidget']">
        <xsl:text>Options: wfoTitle+wfoClose</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="component_header">
    <xsl:text>component "</xsl:text><xsl:value-of select="property[name[text()='name']]/cstring"/><xsl:text>" is </xsl:text><xsl:call-template name="get_classname"/>
  </xsl:template>

  <xsl:template name="get_classname">
    <xsl:choose>
      <xsl:when test="class='QPushButton'">"Command Button"</xsl:when>
      <xsl:when test="class='QRadioButton'">"Radio Button"</xsl:when>
      <xsl:when test="class='QButtonGroup'">"Group Frame"</xsl:when>
      <xsl:when test="class='QGroupBox'">"Group Frame"</xsl:when>
      <xsl:when test="class='QCheckBox'">"Check Box"</xsl:when>
      <xsl:when test="class='QLineEdit'">"Text Box"</xsl:when>
      <xsl:when test="class='QLabel' and property/name='text'">"Label"</xsl:when>
      <xsl:when test="class='QLabel' and property/name='pixmap'">"Image View"</xsl:when>
      <xsl:otherwise><xsl:value-of select="class"/></xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="prefix">
    <xsl:choose>
      <xsl:when test="class='QWidget' or class='QDialog'"><xsl:text>w</xsl:text></xsl:when>
      <xsl:when test="../class='QPushButton' or class='QPushButton'"><xsl:text>b</xsl:text></xsl:when>
      <xsl:when test="../class='QButtonGroup'"><xsl:text>g</xsl:text></xsl:when>
      <xsl:when test="../class='QGroupBox'"><xsl:text>g</xsl:text></xsl:when>
      <xsl:when test="../class='QLabel' and ../property/name='pixmap'"><xsl:text>iv</xsl:text></xsl:when>
      <xsl:when test="../class='QLabel' and ../property/name='text'"><xsl:text>lbl</xsl:text></xsl:when>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="widget"/>
  <xsl:template match="class"/>
  <xsl:template match="property"/>
  <xsl:template match="text()"/>

</xsl:stylesheet>

