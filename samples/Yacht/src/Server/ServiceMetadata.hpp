#pragma once
// The original WCF host generates these documents by reflecting over IYachtService.
// C++ has no such reflection, so preserve its measured responses as AOT contract metadata.
// Reproduce with scripts/check-soap-current.py and this generator.
#include <string_view>

namespace YachtServices::ServiceMetadata {

// original-wsdl.xml: SHA-256 0e75f1c2d57b1e4426805706ebad7b9724e4b9ef916c51c088b579e835a4ad94
inline constexpr std::string_view Wsdl = R"YachtWsdl(<?xml version="1.0" encoding="utf-8"?><wsdl:definitions xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:soapenc="http://schemas.xmlsoap.org/soap/encoding/" xmlns:wsaw="http://www.w3.org/2006/05/addressing/wsdl" xmlns:soap12="http://schemas.xmlsoap.org/wsdl/soap12/" xmlns:wsap="http://schemas.xmlsoap.org/ws/2004/08/addressing/policy" xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing" xmlns:soap="http://schemas.xmlsoap.org/wsdl/soap/" xmlns:wsp="http://schemas.xmlsoap.org/ws/2004/09/policy" xmlns:msc="http://schemas.microsoft.com/ws/2005/12/wsdl/contract" xmlns:wsa10="http://www.w3.org/2005/08/addressing" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd" xmlns:tns="http://tempuri.org/" name="service" targetNamespace="http://tempuri.org/" xmlns:wsdl="http://schemas.xmlsoap.org/wsdl/"><wsdl:types><xsd:schema targetNamespace="http://tempuri.org/Imports"><xsd:import schemaLocation="http://localhost:8888/GameServer/?xsd=" namespace="http://www.w3.org/2001/XMLSchema" /><xsd:import schemaLocation="http://localhost:8888/GameServer/?xsd=xsd0" namespace="http://tempuri.org/" /><xsd:import schemaLocation="http://localhost:8888/GameServer/?xsd=xsd1" namespace="http://schemas.microsoft.com/2003/10/Serialization/" /></xsd:schema></wsdl:types><wsdl:message name="IYachtService_Register_InputMessage"><wsdl:part name="parameters" element="tns:Register" /></wsdl:message><wsdl:message name="IYachtService_Register_OutputMessage"><wsdl:part name="parameters" element="tns:RegisterResponse" /></wsdl:message><wsdl:message name="IYachtService_Unregister_InputMessage"><wsdl:part name="parameters" element="tns:Unregister" /></wsdl:message><wsdl:message name="IYachtService_Unregister_OutputMessage"><wsdl:part name="parameters" element="tns:UnregisterResponse" /></wsdl:message><wsdl:message name="IYachtService_JoinGame_InputMessage"><wsdl:part name="parameters" element="tns:JoinGame" /></wsdl:message><wsdl:message name="IYachtService_JoinGame_OutputMessage"><wsdl:part name="parameters" element="tns:JoinGameResponse" /></wsdl:message><wsdl:message name="IYachtService_LeaveGame_InputMessage"><wsdl:part name="parameters" element="tns:LeaveGame" /></wsdl:message><wsdl:message name="IYachtService_LeaveGame_OutputMessage"><wsdl:part name="parameters" element="tns:LeaveGameResponse" /></wsdl:message><wsdl:message name="IYachtService_GameStep_InputMessage"><wsdl:part name="parameters" element="tns:GameStep" /></wsdl:message><wsdl:message name="IYachtService_GameStep_OutputMessage"><wsdl:part name="parameters" element="tns:GameStepResponse" /></wsdl:message><wsdl:message name="IYachtService_GetGameState_InputMessage"><wsdl:part name="parameters" element="tns:GetGameState" /></wsdl:message><wsdl:message name="IYachtService_GetGameState_OutputMessage"><wsdl:part name="parameters" element="tns:GetGameStateResponse" /></wsdl:message><wsdl:message name="IYachtService_GetAvailableGames_InputMessage"><wsdl:part name="parameters" element="tns:GetAvailableGames" /></wsdl:message><wsdl:message name="IYachtService_GetAvailableGames_OutputMessage"><wsdl:part name="parameters" element="tns:GetAvailableGamesResponse" /></wsdl:message><wsdl:message name="IYachtService_NewGame_InputMessage"><wsdl:part name="parameters" element="tns:NewGame" /></wsdl:message><wsdl:message name="IYachtService_NewGame_OutputMessage"><wsdl:part name="parameters" element="tns:NewGameResponse" /></wsdl:message><wsdl:message name="IYachtService_ResetTimeout_InputMessage"><wsdl:part name="parameters" element="tns:ResetTimeout" /></wsdl:message><wsdl:message name="IYachtService_ResetTimeout_OutputMessage"><wsdl:part name="parameters" element="tns:ResetTimeoutResponse" /></wsdl:message><wsdl:message name="IYachtService_GetScoreCard_InputMessage"><wsdl:part name="parameters" element="tns:GetScoreCard" /></wsdl:message><wsdl:message name="IYachtService_GetScoreCard_OutputMessage"><wsdl:part name="parameters" element="tns:GetScoreCardResponse" /></wsdl:message><wsdl:portType name="IYachtService"><wsdl:operation name="Register"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/Register" message="tns:IYachtService_Register_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/RegisterResponse" message="tns:IYachtService_Register_OutputMessage" /></wsdl:operation><wsdl:operation name="Unregister"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/Unregister" message="tns:IYachtService_Unregister_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/UnregisterResponse" message="tns:IYachtService_Unregister_OutputMessage" /></wsdl:operation><wsdl:operation name="JoinGame"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/JoinGame" message="tns:IYachtService_JoinGame_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/JoinGameResponse" message="tns:IYachtService_JoinGame_OutputMessage" /></wsdl:operation><wsdl:operation name="LeaveGame"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/LeaveGame" message="tns:IYachtService_LeaveGame_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/LeaveGameResponse" message="tns:IYachtService_LeaveGame_OutputMessage" /></wsdl:operation><wsdl:operation name="GameStep"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/GameStep" message="tns:IYachtService_GameStep_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/GameStepResponse" message="tns:IYachtService_GameStep_OutputMessage" /></wsdl:operation><wsdl:operation name="GetGameState"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/GetGameState" message="tns:IYachtService_GetGameState_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/GetGameStateResponse" message="tns:IYachtService_GetGameState_OutputMessage" /></wsdl:operation><wsdl:operation name="GetAvailableGames"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/GetAvailableGames" message="tns:IYachtService_GetAvailableGames_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/GetAvailableGamesResponse" message="tns:IYachtService_GetAvailableGames_OutputMessage" /></wsdl:operation><wsdl:operation name="NewGame"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/NewGame" message="tns:IYachtService_NewGame_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/NewGameResponse" message="tns:IYachtService_NewGame_OutputMessage" /></wsdl:operation><wsdl:operation name="ResetTimeout"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/ResetTimeout" message="tns:IYachtService_ResetTimeout_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/ResetTimeoutResponse" message="tns:IYachtService_ResetTimeout_OutputMessage" /></wsdl:operation><wsdl:operation name="GetScoreCard"><wsdl:input wsaw:Action="http://tempuri.org/IYachtService/GetScoreCard" message="tns:IYachtService_GetScoreCard_InputMessage" /><wsdl:output wsaw:Action="http://tempuri.org/IYachtService/GetScoreCardResponse" message="tns:IYachtService_GetScoreCard_OutputMessage" /></wsdl:operation></wsdl:portType><wsdl:binding name="BasicHttpBinding_IYachtService" type="tns:IYachtService"><soap:binding transport="http://schemas.xmlsoap.org/soap/http" /><wsdl:operation name="Register"><soap:operation soapAction="http://tempuri.org/IYachtService/Register" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="Unregister"><soap:operation soapAction="http://tempuri.org/IYachtService/Unregister" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="JoinGame"><soap:operation soapAction="http://tempuri.org/IYachtService/JoinGame" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="LeaveGame"><soap:operation soapAction="http://tempuri.org/IYachtService/LeaveGame" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="GameStep"><soap:operation soapAction="http://tempuri.org/IYachtService/GameStep" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="GetGameState"><soap:operation soapAction="http://tempuri.org/IYachtService/GetGameState" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="GetAvailableGames"><soap:operation soapAction="http://tempuri.org/IYachtService/GetAvailableGames" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="NewGame"><soap:operation soapAction="http://tempuri.org/IYachtService/NewGame" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="ResetTimeout"><soap:operation soapAction="http://tempuri.org/IYachtService/ResetTimeout" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation><wsdl:operation name="GetScoreCard"><soap:operation soapAction="http://tempuri.org/IYachtService/GetScoreCard" style="document" /><wsdl:input><soap:body use="literal" /></wsdl:input><wsdl:output><soap:body use="literal" /></wsdl:output></wsdl:operation></wsdl:binding><wsdl:service name="service"><wsdl:port name="BasicHttpBinding_IYachtService" binding="tns:BasicHttpBinding_IYachtService"><soap:address location="http://localhost:8888/GameServer/" /></wsdl:port></wsdl:service></wsdl:definitions>)YachtWsdl";

// original-xsd-empty.xml: SHA-256 290a46a96f92da66e07d1e55b5b7153c09516ededf54d313c2e7c846ecb43fe5
inline constexpr std::string_view SchemaHelp = R"YachtSchemaHelp(<?xml version="1.0" encoding="utf-8"?><html>
<head>
<title>Service YachtService</title>
</head>
<body>

<p>To create client proxy source, run:</p>
<p><code>svcutil <a href="http://localhost:8888/GameServer/?wsdl">http://localhost:8888/GameServer/?wsdl</a></code></p>
<!-- FIXME: add client proxy usage (that required decent ServiceContractGenerator implementation, so I leave it yet.) -->

</body>
</html>)YachtSchemaHelp";

// original-xsd0.xml: SHA-256 df2bc23220917aab06d7b795909315cdec392f5e6b624c9af8015a50468e8d01
inline constexpr std::string_view Schema0 = R"YachtSchema0(<?xml version="1.0" encoding="utf-8"?><xs:schema xmlns:tns="http://tempuri.org/" elementFormDefault="qualified" targetNamespace="http://tempuri.org/" xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:import namespace="http://schemas.microsoft.com/2003/10/Serialization/" />
  <xs:element name="Register">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="clientURI" type="xs:anyURI" />
        <xs:element minOccurs="0" name="name" nillable="true" type="xs:string" />
        <xs:element minOccurs="0" name="playerID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="RegisterResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="RegisterResult" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="Unregister">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="UnregisterResponse">
    <xs:complexType>
      <xs:sequence />
    </xs:complexType>
  </xs:element>
  <xs:element name="JoinGame">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="gameID" xmlns:q1="http://schemas.microsoft.com/2003/10/Serialization/" type="q1:guid" />
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="JoinGameResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="JoinGameResult" type="xs:boolean" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="LeaveGame">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="LeaveGameResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="LeaveGameResult" type="xs:boolean" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GameStep">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="gameID" xmlns:q2="http://schemas.microsoft.com/2003/10/Serialization/" type="q2:guid" />
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
        <xs:element minOccurs="0" name="scoreLine" type="xs:int" />
        <xs:element minOccurs="0" name="score" type="xs:unsignedByte" />
        <xs:element minOccurs="0" name="player" type="xs:int" />
        <xs:element minOccurs="0" name="step" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GameStepResponse">
    <xs:complexType>
      <xs:sequence />
    </xs:complexType>
  </xs:element>
  <xs:element name="GetGameState">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="gameID" xmlns:q3="http://schemas.microsoft.com/2003/10/Serialization/" type="q3:guid" />
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GetGameStateResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="GetGameStateResult" type="xs:base64Binary" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GetAvailableGames">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GetAvailableGamesResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="GetAvailableGamesResult" type="xs:base64Binary" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="NewGame">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
        <xs:element minOccurs="0" name="name" nillable="true" type="xs:string" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="NewGameResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="NewGameResult" type="xs:base64Binary" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="ResetTimeout">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="gameID" xmlns:q4="http://schemas.microsoft.com/2003/10/Serialization/" type="q4:guid" />
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="ResetTimeoutResponse">
    <xs:complexType>
      <xs:sequence />
    </xs:complexType>
  </xs:element>
  <xs:element name="GetScoreCard">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="sessionID" type="xs:int" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
  <xs:element name="GetScoreCardResponse">
    <xs:complexType>
      <xs:sequence>
        <xs:element minOccurs="0" name="GetScoreCardResult" type="xs:base64Binary" />
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>)YachtSchema0";

// original-xsd1.xml: SHA-256 cd648643ca15efddacdc60e17c5de6beb41b6d7f73ffa006f403f9616b1bef74
inline constexpr std::string_view Schema1 = R"YachtSchema1(<?xml version="1.0" encoding="utf-8"?><xs:schema xmlns:tns="http://schemas.microsoft.com/2003/10/Serialization/" attributeFormDefault="qualified" elementFormDefault="qualified" targetNamespace="http://schemas.microsoft.com/2003/10/Serialization/" xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="anyType" nillable="true" type="xs:anyType" />
  <xs:element name="anyURI" nillable="true" type="xs:anyURI" />
  <xs:element name="base64Binary" nillable="true" type="xs:base64Binary" />
  <xs:element name="boolean" nillable="true" type="xs:boolean" />
  <xs:element name="byte" nillable="true" type="xs:byte" />
  <xs:element name="dateTime" nillable="true" type="xs:dateTime" />
  <xs:element name="decimal" nillable="true" type="xs:decimal" />
  <xs:element name="double" nillable="true" type="xs:double" />
  <xs:element name="float" nillable="true" type="xs:float" />
  <xs:element name="int" nillable="true" type="xs:int" />
  <xs:element name="long" nillable="true" type="xs:long" />
  <xs:element name="QName" nillable="true" type="xs:QName" />
  <xs:element name="short" nillable="true" type="xs:short" />
  <xs:element name="string" nillable="true" type="xs:string" />
  <xs:element name="unsignedByte" nillable="true" type="xs:unsignedByte" />
  <xs:element name="unsignedInt" nillable="true" type="xs:unsignedInt" />
  <xs:element name="unsignedLong" nillable="true" type="xs:unsignedLong" />
  <xs:element name="unsignedShort" nillable="true" type="xs:unsignedShort" />
  <xs:element name="char" nillable="true" type="tns:char" />
  <xs:simpleType name="char">
    <xs:restriction base="xs:int" />
  </xs:simpleType>
  <xs:element name="duration" nillable="true" type="tns:duration" />
  <xs:simpleType name="duration">
    <xs:restriction base="xs:duration">
      <xs:pattern value="\-?P(\d*D)?(T(\d*H)?(\d*M)?(\d*(\.\d*)?S)?)?" />
      <xs:minInclusive value="-P10675199DT2H48M5.4775808S" />
      <xs:maxInclusive value="P10675199DT2H48M5.4775807S" />
    </xs:restriction>
  </xs:simpleType>
  <xs:element name="guid" nillable="true" type="tns:guid" />
  <xs:simpleType name="guid">
    <xs:restriction base="xs:string">
      <xs:pattern value="[\da-fA-F]{8}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{4}-[\da-fA-F]{12}" />
    </xs:restriction>
  </xs:simpleType>
  <xs:attribute name="FactoryType" type="xs:QName" />
  <xs:attribute name="Id" type="xs:ID" />
  <xs:attribute name="Ref" type="xs:IDREF" />
</xs:schema>)YachtSchema1";

} // namespace YachtServices::ServiceMetadata
