import java.util.regex.*;

String replaceHtmlImgTag(String html) {
       Pattern pattern = Pattern.compile("(<\\s*img[^>]+src\\s*=\\s*['\"])([^'\"]+)(['\"][^>]*/>)");
       StringBuffer sb = new StringBuffer();
       Matcher matcher = pattern.matcher(html);
       while (matcher.find()) {
          matcher.appendReplacement(sb, matcher.group(1) + replace(url) + matcher.group(3));
       }
       matcher.appendTail(sb);
       return sb;
}
