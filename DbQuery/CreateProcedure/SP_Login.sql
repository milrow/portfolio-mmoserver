-- ================================================
-- Template generated from Template Explorer using:
-- Create Procedure (New Menu).SQL
--
-- Use the Specify Values for Template Parameters 
-- command (Ctrl-Shift-M) to fill in the parameter 
-- values below.
--
-- This block of comments will not be included in
-- the definition of the procedure.
-- ================================================
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Author:		KO
-- Create date: 
-- Description:	
-- =============================================
CREATE PROCEDURE SP_Login 
	-- Add the parameters for the stored procedure here
	@LoginID varchar(255), 
	@Password varchar(255)
AS
BEGIN
	SET NOCOUNT ON;

	DECLARE @AccountID bigint;
	DECLARE @RealPassword varchar(255);

	SELECT @AccountID = AccountID, @RealPassword = Password FROM dbo.Account WHERE LoginID = @LoginID;

	IF @@ROWCOUNT = 0
		BEGIN
		SELECT 1 AS ResultCode, 0 AS AccountID;
			RETURN;
		END

	IF @RealPassword <> @Password
		BEGIN
		SELECT 2 AS ResultCode, 0 AS AccountID;
			RETURN;
		END

	SELECT 0 AS ResultCode;
   
END
GO
