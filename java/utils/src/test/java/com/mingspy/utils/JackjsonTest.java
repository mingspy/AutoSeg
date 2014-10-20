package com.mingspy.utils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.codehaus.jackson.JsonGenerationException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;
import org.junit.Assert;
import org.junit.Test;

import com.mongodb.DBObject;
import com.mongodb.util.JSON;

public class JackjsonTest
{

    private static final String SOURCE = "jsonprv([{\"topicid\":2371423,\"replys\":430,\"views\":117789},{\"topicid\":9346362,\"replys\":612,\"views\":270074},{\"topicid\":9964450,\"replys\":1566,\"views\":394678},{\"topicid\":10420856,\"replys\":63,\"views\":18637},{\"topicid\":13187172,\"replys\":3478,\"views\":1226508},{\"topicid\":13302114,\"replys\":280,\"views\":52379},{\"topicid\":15746415,\"replys\":19,\"views\":1819},{\"topicid\":15831148,\"replys\":637,\"views\":132702},{\"topicid\":15935355,\"replys\":659,\"views\":64088},{\"topicid\":16327400,\"replys\":420,\"views\":62042},{\"topicid\":16737348,\"replys\":233,\"views\":40525},{\"topicid\":17635987,\"replys\":14,\"views\":2349},{\"topicid\":19219202,\"replys\":402,\"views\":45275},{\"topicid\":20462656,\"replys\":65,\"views\":1288},{\"topicid\":22219838,\"replys\":801,\"views\":195692},{\"topicid\":22685173,\"replys\":622,\"views\":62610},{\"topicid\":22995079,\"replys\":133,\"views\":22708},{\"topicid\":23276566,\"replys\":647,\"views\":223857},{\"topicid\":23406115,\"replys\":397,\"views\":51212},{\"topicid\":23563711,\"replys\":905,\"views\":141246},{\"topicid\":23834951,\"replys\":83,\"views\":3367},{\"topicid\":23844377,\"replys\":585,\"views\":342932},{\"topicid\":23946810,\"replys\":464,\"views\":10433},{\"topicid\":24074774,\"replys\":102,\"views\":10811},{\"topicid\":24146208,\"replys\":212,\"views\":45653},{\"topicid\":24187014,\"replys\":182,\"views\":5317},{\"topicid\":24320027,\"replys\":4313,\"views\":1632535},{\"topicid\":24376461,\"replys\":275,\"views\":11472},{\"topicid\":24547804,\"replys\":3581,\"views\":909725},{\"topicid\":24674673,\"replys\":140,\"views\":2716},{\"topicid\":25686043,\"replys\":139,\"views\":19882},{\"topicid\":25805790,\"replys\":5702,\"views\":1415275},{\"topicid\":26089740,\"replys\":747,\"views\":11441},{\"topicid\":26111648,\"replys\":133,\"views\":4467},{\"topicid\":27696511,\"replys\":173,\"views\":19386},{\"topicid\":27822098,\"replys\":831,\"views\":59785},{\"topicid\":28212711,\"replys\":2908,\"views\":695602},{\"topicid\":28290703,\"replys\":80,\"views\":2627},{\"topicid\":28298155,\"replys\":2019,\"views\":481643},{\"topicid\":28485361,\"replys\":484,\"views\":30294},{\"topicid\":28509293,\"replys\":3050,\"views\":495684},{\"topicid\":28528344,\"replys\":58,\"views\":1951},{\"topicid\":28608265,\"replys\":96,\"views\":7299},{\"topicid\":28923060,\"replys\":699,\"views\":90278},{\"topicid\":29091327,\"replys\":75,\"views\":2006},{\"topicid\":29222054,\"replys\":14,\"views\":312},{\"topicid\":29300405,\"replys\":247,\"views\":5501},{\"topicid\":29370320,\"replys\":102,\"views\":12112},{\"topicid\":29445188,\"replys\":427,\"views\":266599},{\"topicid\":29747850,\"replys\":45,\"views\":1896},{\"topicid\":29989562,\"replys\":3,\"views\":386},{\"topicid\":30011130,\"replys\":33,\"views\":1508},{\"topicid\":30059800,\"replys\":158,\"views\":8428},{\"topicid\":30247489,\"replys\":201,\"views\":6423},{\"topicid\":30439688,\"replys\":71,\"views\":6369},{\"topicid\":30457654,\"replys\":25,\"views\":1963},{\"topicid\":30512555,\"replys\":128,\"views\":1715},{\"topicid\":30563501,\"replys\":242,\"views\":19821},{\"topicid\":30571467,\"replys\":64,\"views\":7976},{\"topicid\":30755537,\"replys\":20,\"views\":1374},{\"topicid\":30756509,\"replys\":1681,\"views\":530625},{\"topicid\":30917057,\"replys\":58,\"views\":6223},{\"topicid\":30933292,\"replys\":90,\"views\":2070},{\"topicid\":30965112,\"replys\":158,\"views\":2664},{\"topicid\":30969956,\"replys\":768,\"views\":178470},{\"topicid\":31006243,\"replys\":63,\"views\":2622},{\"topicid\":31151925,\"replys\":85,\"views\":2275},{\"topicid\":31176071,\"replys\":195,\"views\":15026},{\"topicid\":31198812,\"replys\":230,\"views\":5243},{\"topicid\":31261673,\"replys\":22,\"views\":2863},{\"topicid\":31287168,\"replys\":55,\"views\":2405},{\"topicid\":31326627,\"replys\":40,\"views\":867},{\"topicid\":31327544,\"replys\":163,\"views\":1762},{\"topicid\":31331551,\"replys\":12,\"views\":650},{\"topicid\":31340291,\"replys\":40,\"views\":1190},{\"topicid\":31355895,\"replys\":23,\"views\":297},{\"topicid\":31363015,\"replys\":15,\"views\":550},{\"topicid\":31393653,\"replys\":30,\"views\":542},{\"topicid\":31408167,\"replys\":98,\"views\":1183},{\"topicid\":31431342,\"replys\":4,\"views\":287},{\"topicid\":31431431,\"replys\":131,\"views\":2429},{\"topicid\":31435687,\"replys\":106,\"views\":5370},{\"topicid\":31439668,\"replys\":212,\"views\":14206},{\"topicid\":31445114,\"replys\":10,\"views\":354},{\"topicid\":31445196,\"replys\":36,\"views\":1174},{\"topicid\":31452985,\"replys\":15,\"views\":696},{\"topicid\":31453130,\"replys\":12,\"views\":415},{\"topicid\":31455930,\"replys\":64,\"views\":1274},{\"topicid\":31464815,\"replys\":23,\"views\":688},{\"topicid\":31500459,\"replys\":157,\"views\":2584},{\"topicid\":31505881,\"replys\":69,\"views\":2143},{\"topicid\":31506961,\"replys\":58,\"views\":918},{\"topicid\":31509962,\"replys\":106,\"views\":18365},{\"topicid\":31511652,\"replys\":38,\"views\":1940},{\"topicid\":31514767,\"replys\":44,\"views\":1282},{\"topicid\":31524579,\"replys\":316,\"views\":17045},{\"topicid\":31548890,\"replys\":25,\"views\":1926},{\"topicid\":31556874,\"replys\":122,\"views\":2391},{\"topicid\":31561009,\"replys\":70,\"views\":775},{\"topicid\":31583972,\"replys\":92,\"views\":2779},{\"topicid\":31598944,\"replys\":245,\"views\":5361},{\"topicid\":31606657,\"replys\":99,\"views\":2441},{\"topicid\":31610782,\"replys\":100,\"views\":1468},{\"topicid\":31621783,\"replys\":161,\"views\":613},{\"topicid\":31623477,\"replys\":43,\"views\":572},{\"topicid\":31627844,\"replys\":14,\"views\":114}])";

    @Test
    public void mongoJSON()
    {
        String jstr = "{" + SOURCE.replace("(", ":").replace(")", "") + "}";
        // jstr = {jsonprv:[{...},{...}]} 可以被解析
        DBObject j = (DBObject) JSON.parse(jstr);
        List<DBObject> list = (List<DBObject>) j.get("jsonprv");

        DBObject first = list.get(0);
        Assert.assertNotNull(first);
        System.out.println(first);
        Assert.assertTrue(first.get("topicid") instanceof Integer);
        Assert.assertTrue(first.get("replys") instanceof Integer);
        Assert.assertTrue(first.get("views") instanceof Integer);
        System.out.println("mongo parse jstring success=>"
                           + jstr.substring(0, 20) + "...");
    }

    public static class RV
    {
        private int topicid;
        private int replys;
        private int views;

        public RV()
        {
        }

        public int getTopicid()
        {
            return topicid;
        }

        public void setTopicid(int topicid)
        {
            this.topicid = topicid;
        }

        public int getReplys()
        {
            return replys;
        }

        public void setReplys(int replys)
        {
            this.replys = replys;
        }

        public int getViews()
        {
            return views;
        }

        public void setViews(int views)
        {
            this.views = views;
        }

    }

    public static class RVList
    {
        public RVList()
        {
        }

        private List<RV> jsonprv = null;

        public List<RV> getJsonprv()
        {
            return jsonprv;
        }

        public void setJsonprv(List<RV> jsonprv)
        {
            this.jsonprv = jsonprv;
        }

    }

    @Test
    public void jackjson() throws JsonGenerationException,
        JsonMappingException, IOException
    {

        // jstr = {jsonprv:[{...},{...}]} 不可以被解析
        String jstr = "{"
                      + SOURCE.replace("jsonprv(", "\"jsonprv\":").replace(")", "")
                      + "}";

        RVList list = null;
        ObjectMapper mapper = new ObjectMapper();
        list = mapper.readValue(jstr, RVList.class);
        Assert.assertNotNull(list);
        Assert.assertNotNull(list.getJsonprv());
        RV first = list.getJsonprv().get(0);

        Assert.assertTrue(first.getReplys() > 0);
        Assert.assertTrue(first.getViews() > 0);
        Assert.assertTrue(first.getTopicid() > 0);
        System.out.println("jackjson parse jstring success=>"
                           + jstr.substring(0, 20) + "...");

    }

}
