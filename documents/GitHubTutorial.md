# GitHub Tutorial
这是单机版GitHub相关知识总结
## 基础知识
GitHub有三个状态区  
- **`工作区`**(Working Directory，VS Code里叫Changes区)-直接编辑的地方，比如记事本打开的文件，肉眼可见，直接操作。add可以把文件增添到暂存区。  
- **`暂存区`**(Stage/Index，VS Code里叫Stages Change区)-数据暂时存放的区域。暂存区的数据可以commit到版本区。  
- **`版本区`**(Commit History)-存放已经commit的数据的区域。push的时候就是把这里的数发到remote repo。   

## 从init开始(本地有一个工作中待上传的文件夹)  
**`1.网站上新建一个new remote repo`**  
**`2.在本地文件夾打开VS Code, 进入terminal，使用如下指令`**   
> git init

(這一步完成後，全部文件轉Changes區)  
**`3.Add所有文件`**  
> git add -A

(這一步完成後，全部文件轉入Staged Changes區)  
**`4.Commit所有文件`**    
> git commit -m "first commit"

(`git log --stat` or `git status`可以查看branch name)  
(這一步完成後，staged區域清空)  
**`5.按照GitHub网站的操作做第一次Push，例如： `**   
> git remote add origin url

> git branch -M main

(如果新建的remote repo不是空的就需要这一步: `git pull --rebase origin main`)  
(這一步完成後，remote repo上的readme.MD或LICENSE就會被同步到本地了)  
> git push -u origin main

如果之前没配置过name和email，则要通过如下指令配置：  
> git config --global user.name "Your Name"

> git config --global user.email you@example.com

(左侧的sync按钮，其实就是sync = pull & push)  
(pull的时候会产生conflict)   
(如果上傳文件大於50mb，是不推薦的。目前來看69.59 MB的文件還是能成功上傳。只是過程中有個warning)  

### 从clone开始(使用已存在的Remote Repo的情況)
登录GitHub账号，并建立一个repo，或选择一个repo。总之，准备好url。  
(新建立的remote repo默认有一个main(而不是master)branch)  
> git clone url
  
(第一次只能clone不能pull url)  
> cd folderName  

> git status  

(这时候会显示nothing to commit, working tree clean)  
> vim fileName  

(在vim里修改了文件)  
> git status  

(显示Changes not staged for commit: modified: <filename>)  
> git commit <filename>  

(会提示输入comments)  
> git status  

(这时候又会显示nothing to commit, working tree clean。但网站并没有更新)  
> git push

(网站上会看到结果)  

## 在网站上修改了，或者换了一台机器，如何同步文件呢
> git pull  

(如果remote repo跟local repo一致，会显示Already up to date)  
(以下两条指令联合使用等同于git pull)  
> git fetch  

> git merge  

(如果local和remote repo是synced，那么git fetch不显示任何结果;否则会显示download信息)  
(以下指令可以Fetch difference到临时branch)  
> git fetch origin main:temp  

> git diff temp  

(这时候会显示remote repo上与local repo上的不同的文件列表)  
> git branch  

(这时候会显示temp branch被创建了，但*还是指向main)  
> git merge temp  

(local repo会看到更新的结果)  

## Local Repo添加了一个新的文件，如何更新到Remote Repo呢 
> git status  

(这时候显示untracked files: newfile.txt)  
(这时候git commit是提示有错误的)  
> git add <filename>  

> git status  

(这时候显示changes to be committed: new file: <filename>)  
(这个命令把文件从workspace推到stage)  
> git commit  

(会提示输入comments)  
> git push  

## 其他有用的GitHub指令
如果commit之後後悔了怎么回退：  
> git reset --soft HEAD^1  

查看项目仓库大小可以使用命令:  
> git count-objects -vH   

如果.git/objects太大了：  
網上有些指令可以瘦身。但也可以重建一個repo（這樣會丟失所有的歷史記錄）  
如果僅僅是把.git/objects裏面的大文件刪除，則會造成無法commit的結果  



## MD Color Solution
```diff
+ Green
- Red
! Orange
@@ Pink @@
# Gray
```
