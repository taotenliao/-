package com.example.eightynumberquestion.Controller;

import org.springframework.web.bind.annotation.*;

import java.io.*;

@RequestMapping("/eight")
@RestController
public class eightNumberController {
    @PostMapping
    String run8Exe(@RequestBody PostObj postObj) throws IOException {
        File file = new File("../softwareEngineeringFront-end/public/8numberArgs.txt");
        if(!file.exists()) {
            return "false";
        }
        FileOutputStream fos = new FileOutputStream(file,false);
        OutputStreamWriter osw = new OutputStreamWriter(fos);
        BufferedWriter bw = new BufferedWriter(osw);
        bw.write(""+postObj.row+'\n');
        bw.write(""+postObj.cl+'\n');
        bw.write(""+postObj.layer+'\n');
        bw.write(postObj.source+'\n');
        bw.write(postObj.dest+'\n');
        bw.newLine();
        bw.flush();
        bw.close();
        osw.close();
        fos.close();

        System.out.println("star");
        Runtime runtime = Runtime.getRuntime();
        String command = "cmd /c start 8numberQuestion.exe";
        String path = "C:\\Users\\86182\\Desktop\\学习\\人工智能导论\\8numberQuestion\\Debug";
        File exeFile = new File(path);
        final Process process = runtime.exec(command, null, exeFile);
        System.out.println("end");
        return "ok";
    }
}
