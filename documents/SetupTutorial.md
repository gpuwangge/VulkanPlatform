# Setup Tutorial

## GitHub

### 基础知识
GitHub有三个状态区  
工作区(Working Directory，也叫Changes区)-直接编辑的地方，比如记事本打开的文件，肉眼可见，直接操作。add可以把文件增添到暂存区。  
暂存区(Stage/Index，也叫Stages Change区)-数据暂时存放的区域。暂存区的数据可以commit到版本区。  
版本区(Commit History)-存放已经commit的数据的区域。push的时候就是把这里的数发到remote repo。   

### 从init开始(本地有待上传的文件夹)  
1.网站上新建一个new remote repo  
2.在本地文件夾打开vs code, 进入terminal，使用如下指令   
**`git init`**
```diff
! git init
```
(這一步完成後，全部文件轉Changes區)  
3.Add所有文件  
**`git add -A`**  
(這一步完成後，全部文件轉入Staged Changes區)  
4.Commit所有文件  
**`git commit -m "first commit"`**    
(`git log --stat` or `git status`可以查看branch name)  
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



## MD Color Solution
```diff
+ Green
- Red
! Orange
@@ Pink @@
# Gray
```
