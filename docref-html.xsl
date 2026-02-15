<?xml version="1.0"?>
<!--
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
-->
<!--
  This XSL stylesheet transforms a muddoc generated documentation of a
  certain header file to an HTML based reference. It will generate the
  following files:
    * files/[<include-path>/]*/<file>.html
    * [<namespace-class>/]*/__index.html
    * [<namespace-xlass>/]*/<method>.html
  The stylesheet is expected to be run against libxslt's xsltproc as it
  uses some specific extensions that are XSL 2.0, but not provided as such
  by libxslt. This include:
    * Using exsl:document instead of xsl:result-document
    * Using func:function instead of xsl:function
    * Using set:distinct instead of xsl:distinct
 -->
<xsl:stylesheet
    version="1.0"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:exsl="http://exslt.org/common"
    xmlns:func="http://exslt.org/functions"
    xmlns:set="http://exslt.org/sets"
    extension-element-prefixes="exsl func set"
    exclude-result-prefixes="xs">

  <xsl:output method="html" indent="yes" omit-xml-declaration="yes"/>

  <!-- Specify the base output folder, typically as a "file://" URI -->
  <xsl:param name="base-dir">file:///tmp/</xsl:param>

  <!-- Specify the base URL. typically a "http://" or "file://" URI -->
  <xsl:param name="base-href">file:///tmp/</xsl:param>

  <!--
    Return the output-folder a '/' separated relative path folder for the
    current element. Each folder element is either a class or namespace
    element of the current node and may include the current node itself as
    well if that node is a 'class'.
    -->
  <func:function name="func:output-folder">
    <xsl:variable name="path-elements">
      <xsl:for-each select="ancestor::namespace | ancestor::class | self::class">
        <xsl:value-of select="concat(@name,'/')"/>
      </xsl:for-each>
    </xsl:variable>
    <func:result select="$path-elements"/>
  </func:function>

  <!--
    Return the fully-qualitied namespace of the current element. Each namespace
    entry is separated by '::' and may be a class or a namespace element. It
    does not include the element itself.
    -->
  <func:function name="func:namespace">
    <xsl:variable name="ns-elements">
      <xsl:for-each select="ancestor::namespace | ancestor::class">
        <xsl:value-of select="concat(@name,'::')"/>
      </xsl:for-each>
    </xsl:variable>
    <func:result select="$ns-elements"/>
  </func:function>

  <!--
    The documentation of the physical file.
    -->
  <xsl:template match="doc">
    <exsl:document
          method="html" index="yes" omit-xml-declaration="yes"
          href="{concat($base-dir, concat(@file, '.html'))}"> 
      <html>
        <head>
          <title><xsl:value-of select="@file"/></title>
          <base>
            <xsl:attribute name="href">
              <xsl:value-of select="$base-href"/>
            </xsl:attribute>
          </base>
          <link rel="stylesheet" href="muddoc.css"/>
        </head>
        <body>
          <h1><xsl:value-of select="@file"/></h1>
          <h2>Classes</h2>
          <table>
            <xsl:apply-templates select ="//class[count(*)&gt;0]"/>
          </table>
        </body>
      </html>
    </exsl:document>
  </xsl:template>

  <!-- Apply the documentation of a class -->
  <xsl:template match="class">
    <!-- Define the file to output the class definition to -->
    <xsl:variable name="output-file">
      <xsl:value-of select="concat($base-dir, concat(func:output-folder(), '_$index$_.html'))"/>
    </xsl:variable>
    <xsl:message>Generating <xsl:value-of select="$output-file"/></xsl:message>

    <!-- Add a link to the parent table -->
    <tr>
      <td>
        <a href="{$output-file}"><xsl:value-of select="@name"/></a>
      </td>
    </tr>

    <!-- Output the class definition -->
    <exsl:document
          method="html" indent="yes" omit-xml-declaration="yes"
          href="{$output-file}">
      <html>
        <head>
          <title><xsl:value-of select="@name"/></title>
          <base>
            <xsl:attribute name="href">
              <xsl:value-of select="$base-href"/>
            </xsl:attribute>
          </base>
          <link rel="stylesheet" href="muddoc.css"/>
        </head>
        <body>
          <h1>
            <span style="font-size:0.66em">
              <xsl:value-of select="func:namespace()"/>
            </span>
            <xsl:value-of select="@name"/>
          </h1>
          <div class="decl"><code><xsl:value-of select="declaration"/></code></div>
          <div class="brief"><xsl:apply-templates select="brief"/></div>
          <div class="detail"><xsl:apply-templates select="detailed"/></div>
          <div class="module">
            <div>Declared in <span>HEADER FILE</span> </div>
            <div>Module <span>LIBRARY FILE</span> </div>
          </div>
          <xsl:choose>
            <xsl:when test="type-definition">
              <h2>Member Types</h2>
              <table>
                <xsl:apply-templates select ="type-definition"/>
              </table>
            </xsl:when>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="method/info[@constructor] | method/info[@destructor]">
              <h2>Construction and Destruction Member Functions</h2>
              <table>
                <xsl:for-each select="set:distinct(method/info[@constructor]/../@name)">
                  <xsl:call-template name="method-group">
                    <xsl:with-param name="name" select="."/>
                    <xsl:with-param name="class" select="../.."/>
                    <xsl:with-param name="brief">Constructors</xsl:with-param>
                  </xsl:call-template>
                </xsl:for-each>
                <xsl:for-each select="set:distinct(method/info[@destructor]/../@name)">
                  <xsl:call-template name="method-group">
                    <xsl:with-param name="name" select="."/>
                    <xsl:with-param name="class" select="../.."/>
                    <xsl:with-param name="brief">Destructor</xsl:with-param>
                  </xsl:call-template>
                </xsl:for-each>
              </table>
            </xsl:when>
          </xsl:choose>
          <xsl:choose>
            <xsl:when test="method/info[not(@constructor) and not(@destructor)]">
              <h2>Member Functions</h2>
              <xsl:value-of select="method/info[not(@constructor) and not(@destructor)]"/>
              <table>
                <xsl:for-each select="set:distinct(method/info[not(@constructor) and not(@destructor)]/../@name)">
                  <xsl:call-template name="method-group">
                    <xsl:with-param name="name" select="."/>
                    <xsl:with-param name="class" select="../.."/>
                  </xsl:call-template>
                </xsl:for-each>
              </table>
            </xsl:when>
          </xsl:choose>
        </body>
      </html>
    </exsl:document>
  </xsl:template>

  <!-- Type Defintions as part of an encompassing table -->
  <xsl:template match="type-definition">
    <tr>
      <td class="decl"><code><xsl:value-of select="declaration"/></code></td>
    </tr>
    <tr>
      <td class="desc"><xsl:apply-templates select="brief"/></td>
    </tr>
  </xsl:template>

  <!--
    Methods with only a brief declaration as part of an encompassing table.
    -->
  <xsl:template match="method" mode="brief-table">
    <tr>
      <td class="decl"><code><xsl:value-of select="declaration"/></code></td>
    </tr>
    <tr>
      <td class="desc"><xsl:apply-templates select="brief"/></td>
    </tr>
  </xsl:template>

  <!--
    A parameter as part of an encompassing table
    -->
  <xsl:template match="param">
    <tr>
      <td class="decl"><code><xsl:value-of select="name"/></code></td>
      <td class="desc"><xsl:apply-templates select="brief"/></td>
    </tr>
  </xsl:template>

  <!--
    A full detailed description of a method.
    -->
  <xsl:template match="method">
    <div class="details">
      <code><xsl:value-of select="declaration"/></code>
      <div class="desc">
        <xsl:choose>
          <xsl:when test="parameters">
            <h3>Parameters</h3>
            <table>
              <xsl:apply-templates select="parameters/param"/>
            </table>
          </xsl:when>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="return">
            <h3>Return</h3>
            <table>
            </table>
          </xsl:when>
        </xsl:choose>
        <td class="desc"><xsl:apply-templates select="detailed"/></td>
      </div>
    </div>
  </xsl:template>

  <!--
    Group methods with the same name as part of an encompassing table.
    -->
  <xsl:template name="method-group">
    <xsl:param name="name"/>
    <xsl:param name="class"/>
    <xsl:param name="brief">
      <xsl:apply-templates select="($class/method[@name=$name])[1]/brief"/>
    </xsl:param>
    <!-- The file for the method group -->
    <xsl:variable name="output-file">
      <xsl:value-of select="concat(concat($base-dir, concat(func:output-folder(), $name)), '.html')"/>
    </xsl:variable>
    <!-- The group method entry in the class table -->
    <tr>
      <td class="decl">
        <a href="{$output-file}">
          <xsl:value-of select="$name"/>
        </a></td>
      <td class="desc"><xsl:value-of select="$brief"/></td>
    </tr>
    <!-- Output the file for the method group -->
    <exsl:document
          method="html" indent="yes" omit-xml-declaration="yes"
          href="{$output-file}">
      <html>
        <head>
          <title><xsl:value-of select="$name"/></title>
          <base>
            <xsl:attribute name="href">
              <xsl:value-of select="$base-href"/>
            </xsl:attribute>
          </base>
          <link rel="stylesheet" href="muddoc.css"/>
        </head>
        <body>
          <h1>
            <span style="font-size:0.66em">
              <xsl:value-of select="func:namespace()"/>
            </span>
            <xsl:value-of select="$name"/>
          </h1>
          <div class="module">
            <div>Declared in <span>HEADER FILE</span> </div>
            <div>Module <span>LIBRARY FILE</span> </div>
            <div>Class <span>CLASS</span> </div>
          </div>
          <h2>Summary</h2>
          <table>
            <xsl:apply-templates select ="$class/method[@name=$name]" mode="brief-table"/>
          </table>
          <h2>Detailed</h2>
          <xsl:apply-templates select ="$class/method[@name=$name]"/>
        </body>
      </html>
    </exsl:document>
  </xsl:template>

  <!--
    Comment text
    -->
  <xsl:template match="brief|detailed">
    <xsl:value-of select="."/>
  </xsl:template>

  <!-- TESTING -->
  <xsl:template match="class" mode="test">
    <xsl:value-of select="@name"/>
    <xsl:text> Ancestors: </xsl:text>
    <xsl:value-of select="func:output-folder()"/>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>

  <!-- Skip anything that does not match -->
  <xsl:template match="*"/>
  
</xsl:stylesheet>
