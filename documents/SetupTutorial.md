# Setup Tutorial

## GitHub
### 从init开始(本地有待上传的文件夹)  
1.网站上新建一个new remote repo  
2.在本地文件夾打开vs code, 进入terminal，使用如下指令  

```diff
+ Green
- Red
! Orange
@@ Pink @@
# Gray
...
```
+ Green
- Red
! Orange
@@ Pink @@
# Gray


::: warning  
*git init*  
:::  
(這一步完成後，全部文件轉Changes區)  
3.Add所有文件
**git add -A**  
(這一步完成後，全部文件轉入Staged Changes區)
4.git commit -m "first commit"
(git log --stat or git status可以查看branch name)
(這一步完成後，staged區域清空)
5.以下是根据github网站的操作：
git remote add origin url
git branch -M main
(git pull --rebase origin main) 如果新建的remote repo不是空的就需要这一步
(這一步完成後，remote repo上的readme.MD或LICENSE就會被同步到本地了)
git push -u origin main


 git config --global user.name "Your Name"
 git config --global user.email you@example.com


(左侧的sync按钮，其实就是sync = pull & push)
(pull的时候会产生conflict)

(如果上傳文件大於50mb，是不推薦的。目前來看69.59 MB的文件還是能成功上傳。只是過程中有個warning)



## VS Code

## Cmake
