
import com.zorbaxquery.exi.exificient_options;
import com.zorbaxquery.exi.exificient_stub;

public class test_main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		exificient_options options = new exificient_options();
		
		options.is_fragment = true;
    	options.preserve_prefixes = true;
		
		try{
			String[] xmls = {"<a/>","<b/>"};
			byte[] exi = exificient_stub.encodeSchemaInformed(xmls, options);
        	System.out.println("result len " + exi.length);
        	String back_result = exificient_stub.decodeSchemaInformed(exi, options);
        	System.out.println("back result " + back_result);
    	} catch (Exception e) {
        	System.out.println("Eception");
        	e.printStackTrace();
    	}
	}

}
